#include <bip47/notification.hpp>
#include <bip47/payment_code.hpp>
#include <bip47/low.hpp>
#include <bip47/patterns.hpp>

namespace bip47
{

namespace notifications
{

// TODO
void sign_transaction(transaction& incomplete, const ec_private& key);

//TODO should we check for anything other than pay to pubkey and pay to pubkey hash?
const outpoint find_redeemable_output(const transaction& tx, const ec_private& pk) {
    const auto pubkey = pk.to_public();
    const address address = pk.to_payment_address();
    for (auto o : tx.outputs()) {
        const auto ops = o.script().operations();
        if (libbitcoin::chain::script::is_pay_key_hash_pattern(ops)) {
            // TODO Is the address one we can redeem? 
        } else if (libbitcoin::chain::script::is_pay_public_key_pattern(ops)) {
            // TODO Is the public key one we can redeem?
        }
    }
    return outpoint();
}

const transaction notify_v1_and_v2(
    const payment_code& alice, 
    const payment_code& bob,
    const output opa, 
    const output opb, 
    const ec_private& designated,
    const outpoint& prior, 
    unsigned int amount,
    const transaction::outs& other_outputs)
{
    // TODO policy as to ordering of outputs?
    transaction::outs outputs(other_outputs.size() + 2);
    outputs.push_back(opa);
    outputs.push_back(opb);
    for (output n : other_outputs) {
        outputs.push_back(n);
    }
    // TODO insert input for signing.
    transaction nt(1, 0, {}, outputs);
    sign_transaction(nt, designated);
    return nt;
}

namespace v1
{

const transaction inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_private& designated,
    const outpoint& prior, 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return notify_v1_and_v2(alice, bob, 
        bip47::low::pay_key_hash(bob, amount, format), 
        bip47::low::v1::notification_output(alice, bob, prior, designated), 
        designated, prior, amount, other_outputs);
}

const transaction inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_private& designated,
    const transaction& prior, 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return notify(alice, bob, designated, find_redeemable_output(prior, designated), format, amount, other_outputs);
}

bool inline valid(const transaction& tx)
{
    for (auto output : tx.outputs()) if (low::v1::is_notification_output(output)) return true;
    return false;
}

bool inline to(const transaction& tx, const address& notification_address) {
    for (auto output : tx.outputs()) if (low::to(output, notification_address)) return true;     
    return false;
}

} // v1

namespace v2
{

const transaction inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_private& designated,                // The private key used to redeem the prior transaction. 
    const outpoint& prior,                       // outpoint to the prior transaction containing the designated pubkey. 
    unsigned int amount,
    const transaction::outs other_outputs) 
{
    return notify_v1_and_v2(alice, bob, 
        low::v2::notification_change_output(alice.point(), bob, amount), 
        low::v1::notification_output(alice, bob, prior, designated), 
        designated, prior, amount, other_outputs);
}

const transaction inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_private& designated,
    const transaction& prior, 
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return notify(alice, bob, designated, find_redeemable_output(prior, designated), amount, other_outputs);
}

bool valid(const transaction& tx) {
    if (!v1::valid(tx)) return false;
    for (auto output : tx.outputs()) {
        if (!low::v2::is_notification_change_output(output)) continue;
        else return true;
    }
    
    return false;
}

bool to(const transaction& tx, const identifier& bob) {
    if (!v1::valid(tx)) return false;
    for (auto output : tx.outputs()) if (low::v2::is_notification_change_output_to(output, bob)) return true;
    return false;
}
    
} // v2

namespace v3
{

typedef libbitcoin::ec_uncompressed ec_uncompressed;

bool is_notification_pattern(const libbitcoin::machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 3 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 3) return false;
    
    // Are the first two keys compressed? 
    if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
    if (ops[2].data().size() != libbitcoin::ec_compressed_size) return false;
    
    // Is the last key uncompressed? 
    if (ops[3].data().size() != libbitcoin::ec_uncompressed_size) return false;
    
    // TODO Is ordering important? 
    
    return true;
}

bool inline valid(const output& output) {
    return is_notification_pattern(output.script().operations());
}

const output inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    unsigned int amount, 
    const ec_private& designated, 
    const outpoint& prior) {
    // TODO randomization/ordering policy? 
    return output(amount,
        libbitcoin::chain::to_pay_multisig_pattern(1,
            {libbitcoin::to_chunk(designated.to_public()),
                libbitcoin::to_chunk(bob.identifier()),
                alice.mask(designated, bob.point(), prior).hd_public_key().data()}));
}

bool to(const transaction& tx, const ec_compressed& bob_id)
{
    for (auto output : tx.outputs()) if (valid(output)) {
        auto ops = output.script().operations();
        
        if (low::identifier_equals(bob_id, ops[1].data())) return true;
    }
    
    return false;
}

bool read(const ec_uncompressed& payload, const transaction& tx, const ec_compressed& bob_id)
{
    for (auto output : tx.outputs()) if (valid(output)) {
        auto ops = output.script().operations();
        
        if (low::identifier_equals(bob_id, ops[1].data())) {
            std::copy(ops[3].data().begin(), ops[3].data().end(), payload.begin());
            return true;
        }
    }
    
    return false;
}

} // v3

} // notifications

} // bip47
