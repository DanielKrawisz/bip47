#include <bip47/versions.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include "designated.cpp"

namespace bip47
{
    
inline const output pay(const payment_code& to, unsigned int amount, address_format format) {
    return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(to.notification_address(format))));
}

inline const bool to(const output& output, const address& notification_address) {
    const auto ops = output.script().operations();
    return (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == notification_address);
}
    
namespace v1
{

inline const output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_private& designated) {
    return output(0, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_null_data_pattern(alice.mask(designated, bob.point(), prior))));
}

const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const outpoint& prior, 
    const ec_private& designated,
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    // TODO policy as to ordering of outputs?
    transaction::outs outputs(other_outputs.size() + 2);
    outputs.push_back(pay(bob, amount, format));
    outputs.push_back(notification_output(alice, bob, prior, designated));
    for (output n : other_outputs) {
        outputs.push_back(n);
    }
    // TODO insert input for signing. 
    return transaction(1, 0, {}, outputs);
}

bool extract_payload(payment_code* payload, const transaction& tx);

bool extract_payload(payment_code* payload, const transaction& tx)
{
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (!libbitcoin::chain::script::is_pay_null_data_pattern(ops)) continue;
        if (ops[1].data().size() == 80) {
            if (payload != 0) *payload = ops[1].data();
            return true;
        }
    }
    
    return false;
}

bool inline valid_notification(const transaction& tx)
{
    return extract_payload(0, tx);
}

bool notification_to(payment_code* payload, const transaction& tx, const address& recipient) {
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == recipient) return extract_payload(payload, tx);
    }
    
    return false;
}

bool inline designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt) {
    return bip47::designated_pubkey(out, previous, nt);
}

} // v1

bool is_notification_change_output_pattern(const libbitcoin::machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 2 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 2) return false;
    
    // TODO I don't know if anything else goes in here. 
    
    return true;
}

namespace v2
{

void identifier(ec_compressed& id, const payment_code& code) {
    auto hash = libbitcoin::sha256_hash(code);
    id[0] = 0x02;
    std::copy(hash.begin(), hash.end(), id.at(1));
}

inline bool is_notification_change_output(const output& output) {
    return is_notification_change_output_pattern(output.script().operations());
}

output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount) {
    ec_compressed bob_id;
    identifier(bob_id, bob);
    // TODO randomization/ordering policy? 
    return output(amount, libbitcoin::chain::script::to_pay_multisig_pattern(1, {alice, bob_id}));
}

const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const outpoint& prior, 
    const ec_private& designated,
    unsigned int amount,
    const transaction::outs other_outputs) 
{
    // TODO policy as to ordering of outputs?
    transaction::outs outputs(other_outputs.size() + 2);
    outputs.push_back(notification_change_output(alice.point(), bob, amount));
    outputs.push_back(v1::notification_output(alice, bob, prior, designated));
    for (output n : other_outputs) {
        outputs.push_back(n);
    }
    // TODO insert input for signing. 
    return transaction(1, 0, {}, outputs);
}

bool valid_notification(const transaction& tx) {
    if (!v1::valid_notification(tx)) return false;
    for (auto output : tx.outputs()) {
        if (!is_notification_change_output(output)) continue;
        else return true;
    }
    
    return false;
}

bool inline designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt) {
    return bip47::designated_pubkey(out, previous, nt);
}

} // v2

bool is_bip47_v3_multisig_pattern(const libbitcoin::machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 3 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 3) return false;
    
    // Are the first two keys compressed? 
    if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
    if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
    
    // Is the last key uncompressed? 
    if (ops[1].data().size() != libbitcoin::ec_uncompressed_size) return false;
    
    return true;
}

bool v3::valid_notification(const transaction& tx)
{
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (!is_bip47_v3_multisig_pattern(ops)) return false;
    }
    
    return true;
}

}
