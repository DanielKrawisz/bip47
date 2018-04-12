#ifndef BIP47_VERSIONS_HPP
#define BIP47_VERSIONS_HPP

#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bip47/payment_code.hpp>

using namespace libbitcoin;

namespace bip47
{

typedef libbitcoin::chain::transaction transaction;

namespace v1
{
const transaction notify(
    const payment_code& from, 
    const ec_compressed& to, 
    const outpoint& prior, 
    const ec_private& designated, 
    unsigned int amount);

bool valid(const transaction &tx);
bool notification_to(payment_code* payload, const transaction& tx, const address& recipient);
    
bool designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt);
} // v1

namespace v2
{
const transaction notify(
    const payment_code& from, 
    const ec_compressed& to, 
    const outpoint& prior, 
    const ec_private& designated, 
    unsigned int amount);

bool valid(const transaction &tx);

bool designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt);
} // v2

namespace v3
{
const transaction notify(
    const payment_code& from, 
    const ec_compressed& to, 
    const outpoint& prior, 
    const ec_private& designated, 
    unsigned int amount);

bool valid(const transaction &tx);
} // v3

} // bip47

#endif
