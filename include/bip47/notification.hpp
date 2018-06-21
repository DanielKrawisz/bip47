#ifndef BIP47_NOTIFICATION_HPP
#define BIP47_NOTIFICATION_HPP

#include <vector>
#include <bip47/secret.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>

namespace bip47
{

typedef libbitcoin::chain::output output;
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::wallet::ec_public ec_public;

namespace notifications
{

namespace v1
{

bool is_notification_output(const output& output);

// Create a v1 notification transaction.  
const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,                // The private key used to redeem the prior transaction. 
    const outpoint& prior,                       // outpoint to the prior transaction containing the designated pubkey. 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs={}
);

// Create a v1 notification tx from a previous tx. 
const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,                // The private key used to redeem the prior transaction. 
    const transaction& prior,                    // A transaction with an output that can be redeemed with the key. 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs={}
);

bool valid(const transaction &tx);

bool to(const transaction& tx, const address& notification_address);

bool read(payment_code& out, const std::vector<transaction>& previous, const transaction& tx, const notification_key& notification_address);

} // v1

namespace v2
{

// Create a v2 notification transaction. 
const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,                // The private key used to redeem the prior transaction. 
    const outpoint& prior,                       // outpoint to the prior transaction containing the designated pubkey. 
    unsigned int amount,
    const transaction::outs other_outputs);

// Create a v2 notification transaction from a previous tx. 
const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,                // The private key used to redeem the prior transaction. 
    const address_format format,
    const transaction& prior,                    // A transaction with an output that can be redeemed with the key. 
    unsigned int amount,
    const transaction::outs other_outputs);

bool valid(const transaction &tx);

bool to(const transaction& tx, const payment_code_identifier& bob);

bool read(payment_code& out, const std::vector<transaction>& previous, const transaction& tx, const payment_code_identifier& bob);

} // v2

namespace v3
{  

bool valid(const output& output);

// TODO do we really need prior? That doesn't really make sense. 
const output notify(
    const ec_secret& designated, 
    const payment_code& bob, 
    unsigned int amount, 
    const outpoint& prior);

bool to(const transaction& tx, const ec_compressed& bob);

bool read(payment_code& payload, const transaction& tx, const ec_compressed& bob);

} // v3
    
} // notifications

namespace low
{

bool to(const output& output, const address& notification_address);

const outpoint find_redeemable_output(const transaction& tx, const ec_secret& pk, address_format format);

const transaction notify_v1_and_v2(
    const payment_code& alice, 
    const payment_code& bob,
    const output opa, 
    const output opb, 
    const ec_secret& designated,
    const outpoint& prior, 
    unsigned int amount,
    const transaction::outs& other_outputs);

inline const output pay_key_hash(const payment_code& to, unsigned int amount, address_format format) {
    return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(notification_address(to, format))));
}

bool identifier_equals(const ec_compressed &x, const data_chunk& y);

inline const address to_payment_address(const ec_secret& key, const address_format format) {
    return libbitcoin::wallet::ec_private(key, format).to_payment_address();
}

bool read_notification_payload(payment_code& out, const output& output);

bool designated_pubkey(ec_public& designated, const std::vector<transaction>& previous, const transaction& nt);

bool unmask_payment_code(payment_code& code, const ec_secret& pk, const ec_public& pubkey, const outpoint& outpoint);

namespace v1
{

inline const output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_secret& designated) {
    return output(0, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_null_data_pattern(mask_payment_code(alice, payment_code_mask(designated, point(bob), prior)))));
}

bool is_notification_change_output_pattern(const libbitcoin::machine::operation::list& ops);

} // v1

namespace v2
{

const output notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount);

inline bool is_notification_change_output(const output& output) {
    return v1::is_notification_change_output_pattern(output.script().operations());
}

const bool is_notification_change_output_to(const output& output, const payment_code_identifier& bob_id);

} // v2

} // low

namespace v1
{

inline const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,
    const outpoint& prior, 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return low::notify_v1_and_v2(alice, bob, 
        bip47::low::pay_key_hash(bob, amount, format), 
        bip47::low::v1::notification_output(alice, bob, prior, designated), 
        designated, prior, amount, other_outputs);
}

inline const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,
    const transaction& prior, 
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return notify(alice, bob, designated, low::find_redeemable_output(prior, designated, format), format, amount, other_outputs);
}

} // v1

namespace v2
{

inline const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,                // The private key used to redeem the prior transaction. 
    const outpoint& prior,                       // outpoint to the prior transaction containing the designated pubkey. 
    unsigned int amount,
    const transaction::outs other_outputs) 
{
    return low::notify_v1_and_v2(alice, bob,
        low::v2::notification_change_output(alice.point(), bob, amount), 
        low::v1::notification_output(alice, bob, prior, designated), 
        designated, prior, amount, other_outputs);
}

inline const transaction notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const ec_secret& designated,
    const address_format format, 
    const transaction& prior, 
    unsigned int amount,
    const transaction::outs other_outputs)
{
    return notify(alice, bob, designated, low::find_redeemable_output(prior, designated, format), amount, other_outputs);
}

} // v2

} // bip47

#endif
