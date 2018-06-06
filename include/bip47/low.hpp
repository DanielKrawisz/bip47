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
    
const output pay_key_hash(const payment_code& to, unsigned int amount, address_format format);

const bool to(const output& output, const address& notification_address);

bool designated_pubkey(ec_public& designated, std::vector<transaction> previous, const transaction& nt);

bool identifier_equals(const ec_compressed &x, const data_chunk& y);

address to_payment_address(const ec_secret& key, const address_format format);

// v1 has functions specific to version 1 payment codes. 
namespace v1
{

const output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_secret& designated);

bool is_notification_output(const output& output);

bool notification_payload(payment_code& payload, const transaction& tx);

// Write the designated pubkey to out given a transaction and a set of previous transactions which may contain the designated
// pubkey. Return whether the designated_pubkey could be read. 
bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);

} // v1

// v2 has functions specific to version 2 payment codes. 
namespace v2
{

bool is_notification_change_output(const output& output);

const output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount);

const bool is_notification_change_output_to(const output& output, const payment_code_identifier& bob_id);

bool notification_payload(payment_code& payload, const transaction& tx);

bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);

} // v2

} // low

} // bip47

#endif
