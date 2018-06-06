#ifndef BIP47_SECRET_HPP
#define BIP47_SECRET_HPP

#include <bip47/payment_code.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace bip47
{

// secret represents a payment code with its private key. 
class secret {
public:
    hd_secret key;
    payment_code code;
    
    secret(payment_code_version version, bool bitmessage_notification, const hd_secret& pk):key(pk), code(version, bitmessage_notification, pk.pubkey()) {};
    secret(payment_code_version version, const hd_secret& pk):secret(version, false, pk){}
    
    const bool valid() {
        return key.valid() && code.valid();
    }

    // Derive address n to another payment code. It is possible that address n doesn't exist, so 
    // increment n if not. The value of n will be the next value that should be used the next
    // time next_address is called. next_address will always return a valid address. 
    const address next_address(unsigned int& n, const payment_code& to) const;
};

} // bip47

#endif
