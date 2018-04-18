#include <bip47/versions.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include "designated.cpp"

namespace bip47
{
    
const inline output pay(const payment_code& to, unsigned int amount, address_format format) {
    return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(to.notification_address(format))));
}

const inline bool to(const output& output, const address& notification_address) {
    const auto ops = output.script().operations();
    return (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == notification_address);
}

const bool payload(payment_code& out, const output& output) {
    const auto ops = output.script().operations();
    if (!libbitcoin::chain::script::is_pay_null_data_pattern(ops)) return false;
    if (ops[1].data().size() != 80) return false;
    out = payment_code(ops[1].data());
    return true;
}
    
namespace v1
{

const inline output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_private& designated) {
    return output(0, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_null_data_pattern(alice.mask(designated, bob.point(), prior))));
}

bool inline is_notification_output(const output& output) {
    const auto ops = output.script().operations();
    return libbitcoin::chain::script::is_pay_null_data_pattern(ops) && ops[1].data().size() == 80;
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

bool inline notification_payload(payment_code& payload, const transaction& tx) {
    for (auto output : tx.outputs()) if (bip47::payload(payload, output)) return true;
    return false;
}

bool inline valid_notification(const transaction& tx)
{
    for (auto output : tx.outputs()) if (is_notification_output(output)) return true;
    return false;
}

bool inline notification_to(const transaction& tx, const address& notification_address) {
    for (auto output : tx.outputs()) if (to(output, notification_address)) return true;     
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
    if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
    if (ops[2].data().size() != libbitcoin::ec_compressed_size) return false;
    
    return true;
}

bool inline identifier_equals(const ec_compressed &x, const data_chunk& y) {
    if (y.size() != libbitcoin::ec_compressed_size) return false;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) if (x[i] != y[i]) return false;
    return true;
}

namespace v2
{

void inline identifier(ec_compressed& id, const payment_code& code) {
    auto hash = libbitcoin::sha256_hash(code);
    id[0] = 0x02;
    std::copy(hash.begin(), hash.end(), id.at(1));
}

bool inline is_notification_change_output(const output& output) {
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

const inline bool is_notification_change_output_to(const output& output, const ec_compressed& bob_id) {
    if (!is_notification_change_output(output)) return false;
    identifier_equals(bob_id, output.script().operations()[1].data());
    return true;
}

bool notification_to(const transaction& tx, const ec_compressed& bob_id) {
    if (!v1::valid_notification(tx)) return false;
    for (auto output : tx.outputs()) if (is_notification_change_output_to(output, bob_id)) return true;
    return false;
}

bool inline notification_payload(payment_code& payload, const transaction& tx) {
    return v1::notification_payload(payload, tx);
}

bool inline designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt) {
    return bip47::designated_pubkey(out, previous, nt);
}

} // v2

namespace v3
{

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

bool inline is_notification_output(const output& output) {
    return is_notification_pattern(output.script().operations());
}

bool notification_to(const ec_uncompressed* payload, const transaction& tx, const ec_compressed& bob_id)
{
    for (auto output : tx.outputs()) if (is_notification_output(output)) {
        auto ops = output.script().operations();
        
        if (identifier_equals(bob_id, ops[1].data())) {
            if (payload != 0) std::copy(ops[3].data().begin(), ops[3].data().end(), payload->begin());
            return true;
        }
    }
    
    return false;
}

} // v3

} // bip47
