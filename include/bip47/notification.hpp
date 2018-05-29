#ifndef BIP47_NOTIFICATION_HPP
#define BIP47_NOTIFICATION_HPP

#include <vector>
#include <bip47/payment_code.hpp>
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

bool read(payment_code& out, const std::vector<const transaction>& previous, const transaction& tx, const address& notification_address);

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

bool to(const transaction& tx, const identifier& bob);

bool read(payment_code& out, const std::vector<transaction>& previous, const transaction& tx, const identifier& bob);

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

}

#endif
