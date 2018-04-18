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
    
struct notification {
public:
    static payment_code_version version(const transaction& tx);
    static bool valid(const transaction& tx);
    static bool designated_pubkey(ec_public& designated, std::vector<transaction> previous, const transaction& nt);
    
    static const transaction notify(
        const payment_code& alice, 
        const payment_code& bob, 
        const transaction& prior, 
        const ec_private& designated,
        address_format format,
        unsigned int amount,
        const transaction::outs other_outputs={});
    
    const transaction& tx;
    
    notification(const transaction& tx);
    
    int version() const;
    bool valid() const;
    bool designated_pubkey(ec_public& designated, std::vector<transaction> previous) const;

private:
    static const outpoint find_redeemable_output(const transaction& tx, const ec_private& pk);
};

}

#endif
