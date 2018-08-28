#include <bip47/notification.hpp>
#include <bip47/patterns.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace bip47
{

// TODO
void sign_transaction(transaction& incomplete, const ec_secret& key) {
    throw 0;
}

namespace low
{

//TODO should we check for anything other than pay to pubkey and pay to pubkey hash?
// There should also be multisig and pay to script hash versions of each. 
const outpoint find_redeemable_output(const transaction& tx, const ec_secret& pk, address_format format) {
    const auto pubkey = low::to_public(pk);
    const address address = low::to_payment_address(pk, format);
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
    const ec_secret& designated,
    const outpoint& prior, 
    unsigned int amount,
    const transaction::outs& other_outputs)
{
    // TODO policy as to ordering of outputs?
    // Any such policy is independent of this library. 
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

} // low

namespace notifications
{

namespace v1
{

bool is_notification_output(const output& output) {
    const auto ops = output.script().operations();
    return libbitcoin::chain::script::is_null_data_pattern(ops) && ops[1].data().size() == 80;
}

bool valid(const transaction& tx)
{
    for (const auto output : tx.outputs()) if (is_notification_output(output)) return true;
    return false;
}

bool to(const transaction& tx, const address& notification_address) {
    for (const auto output : tx.outputs()) if (low::to(output, notification_address)) return true;     
    return false;
}

bool read(payment_code& out, const transaction& tx, const blockchain& b, const notification_key& notification) {
    // Is this a transaction to the notification address? 
    if (!to(tx, notification.notification_address)) return false;
    
    // Find designated pubkey. 
    ec_public designated;
    outpoint op;
    if (!low::designated_pubkey_and_outpoint(designated, op, b, tx)) return false;
    
    // Find a valid notification output. 
    for (const auto output : tx.outputs()) if (low::read_notification_payload(out, output)) {
        // If this is not a version 1 payment code continue. 
        if (out.version() != 1) continue;
        
        if (!low::unmask_payment_code(out, designated, notification.secret.Secret, op)) continue;
        
        if (!abstractions::hd::bip32::valid_public_key(out.point())) continue;
    }
    
    return false;
}

} // v1

namespace v2
{

bool valid(const transaction& tx) {
    if (!v1::valid(tx)) return false;
    for (auto output : tx.outputs()) {
        if (!low::v2::is_notification_change_output(output)) continue;
        else return true;
    }
    
    return false;
}

bool to(const transaction& tx, const payment_code_identifier& bob) {
    if (!v1::valid(tx)) return false;
    for (auto output : tx.outputs()) if (low::v2::is_notification_change_output_to(output, bob)) return true;
    return false;
}

// TODO
bool read(payment_code& out, const transaction& tx, const blockchain&, const notification_key& notification) {
    throw 0;
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

/*
const output inline notify(
    const payment_code& alice, 
    const payment_code& bob, 
    unsigned int amount, 
    const ec_secret& designated, 
    const outpoint& prior) {
    // TODO randomization/ordering policy? 
    return output(amount,
        libbitcoin::chain::to_pay_multisig_pattern(1,
            {libbitcoin::to_chunk(low::to_public(designated)),
                libbitcoin::to_chunk(bob.identifier()),
                low::masked_pubkey(alice, low::payment_code_mask(designated, bob.point(), prior))}));
}
*/

bool to(const transaction& tx, const ec_compressed& bob_id)
{
    for (auto output : tx.outputs()) if (valid(output)) {
        auto ops = output.script().operations();
        
        if (low::identifier_equals(bob_id, ops[1].data())) return true;
    }
    
    return false;
}

bool read(ec_uncompressed& payload, const transaction& tx, const ec_compressed& bob_id)
{
    for (auto output : tx.outputs()) if (valid(output)) {
        auto ops = output.script().operations();
        
        if (low::identifier_equals(bob_id, ops[1].data())) {
            for (int i = 0; i < payload.size(); i++) {
                payload[i] = ops[3].data()[i];
            }
            std::copy(ops[3].data().begin(), ops[3].data().end(), payload.begin());
            return true;
        }
    }
    
    return false;
}

} // v3

} // notifications

} // bip47
