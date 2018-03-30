#ifndef BIP47_SECRET_HPP
#define BIP47_SECRET_HPP

#include <bip47/payment_code.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace bip47
{
    
typedef libbitcoin::wallet::hd_private hd_private;

struct secret {
public:
    secret(int version, const hd_private& key, bool bitmessage_notification);
    
    const hd_private& key;
    const payment_code code;
};

} // bip47

#endif
