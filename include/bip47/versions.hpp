#ifndef BIP47_VERSIONS_HPP
#define BIP47_VERSIONS_HPP

#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bip47/payment_code.hpp>

namespace bip47
{

typedef libbitcoin::chain::output output;
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::wallet::ec_public ec_public;
typedef libbitcoin::ec_uncompressed ec_uncompressed;

namespace v1
{

const output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_private& designated);

bool is_notification_output(const output& output);

const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const outpoint& prior, 
    const ec_private& designated,
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs);

bool valid_notification(const transaction &tx);

bool notification_to(const transaction& tx, const address& notification_address);

bool notification_payload(payment_code& payload, const transaction& tx);
    
bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);
} // v1

namespace v2
{

void identifier(ec_compressed& id, const payment_code& bob);

bool is_notification_change_output(const output& output);

output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount);
    
const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const outpoint& prior, 
    const ec_private& designated,
    unsigned int amount,
    const transaction::outs other_outputs);

bool valid_notification(const transaction &tx);

bool notification_to(const transaction& tx, const ec_compressed& bob_id);

bool notification_payload(payment_code& payload, const transaction& tx);

bool designated_pubkey(ec_public& out, const std::vector<transaction>& previous, const transaction& nt);

} // v2

namespace v3
{  

bool notification_to(const ec_uncompressed* payload, const transaction& tx, const ec_compressed& bob_id);

} // v3

} // bip47

#endif
