#ifndef BIP47_SECRET_HPP
#define BIP47_SECRET_HPP

#include <bip47/payment_code.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace bip47
{

struct secret {
public:
    secret(uint8_t version, const ec_private& pk, const hd_chain_code& chain_code, bool bitmessage_notification);

    const address next_address(unsigned int& n, const payment_code& to) const;
    
    const ec_private& key;
    const payment_code code;
};

} // bip47

#endif
