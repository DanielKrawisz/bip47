#ifndef BIP47_NOTIFICATION_HPP
#define BIP47_NOTIFICATION_HPP

#include <vector>
#include <bip47/payment_code.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>

namespace bip47
{
    
typedef libbitcoin::chain::transaction transaction;
    
struct notification {
public:
    static uint8_t version(const transaction& tx);
    static bool valid(const transaction& tx);
    static bool designated_pubkey(data_chunk& designated, std::vector<transaction> previous, const transaction& nt);
    
    static const transaction notify(
        const payment_code& from, 
        const payment_code& to, 
        const transaction& prior, 
        const ec_private& designated,
        unsigned int amount);
    
    const transaction& tx;
    
    notification(const transaction& tx);
    
    int version() const;
    bool valid() const;
    bool designated_pubkey(data_chunk& designated, std::vector<transaction> previous) const;

private:
    static const outpoint find_redeemable_output(const transaction& tx, const ec_private& pk);
};

}

#endif
