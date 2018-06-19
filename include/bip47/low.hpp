#ifndef BIP47_LOW_HPP
#define BIP47_LOW_HPP

#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bip47/payment_code.hpp>

namespace bip47
{

typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::wallet::ec_public ec_public;
typedef libbitcoin::chain::output output;
typedef libbitcoin::chain::transaction transaction;

// low contains functions which are probably too low-level for most ordinary use cases 
// but which are still exposed publicly just in case. 
namespace low
{

inline const output pay_key_hash(const payment_code& to, unsigned int amount, address_format format) {
    return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(notification_address(to, format))));
}

inline const bool to(const output& output, const address& notification_address) {
    const auto ops = output.script().operations();
    return (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == notification_address);
}

inline const address to_payment_address(const ec_secret& key, const address_format format) {
    return libbitcoin::wallet::ec_private(key, format).to_payment_address();
}

bool designated_pubkey(ec_public& designated, std::vector<transaction> previous, const transaction& nt);

bool identifier_equals(const ec_compressed &x, const data_chunk& y);

bool read_payload(payment_code& out, const output& output);

// v1 has functions specific to version 1 payment codes. 
namespace v1
{

const output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_secret& designated);

inline bool is_notification_output(const output& output) {
    const auto ops = output.script().operations();
    return libbitcoin::chain::script::is_pay_null_data_pattern(ops) && ops[1].data().size() == 80;
}

inline bool notification_payload(payment_code& payload, const transaction& tx) {
    for (auto output : tx.outputs()) if (read_payload(payload, output)) return true;
    return false;
}

// Write the designated pubkey to out given a transaction and a set of previous transactions which may contain the designated
// pubkey. Return whether the designated_pubkey could be read. 
bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);

bool is_notification_change_output_pattern(const libbitcoin::machine::operation::list& ops);

} // v1

// v2 has functions specific to version 2 payment codes. 
namespace v2
{

inline bool is_notification_change_output(const output& output) {
    return v1::is_notification_change_output_pattern(output.script().operations());
}

const output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount);

const bool is_notification_change_output_to(const output& output, const payment_code_identifier& bob_id);

inline bool notification_payload(payment_code& payload, const transaction& tx) {
    return v1::notification_payload(payload, tx);
}

bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);

} // v2

} // low

} // bip47

#endif
