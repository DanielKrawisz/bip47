#ifndef BIP47_SECRET_HPP
#define BIP47_SECRET_HPP

#include <bip47/payment_code.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace bip47
{

struct notification_key {
    hd_secret secret;
    address notification_address;
};

// secret represents a payment code with its private key. 
struct secret {
    hd_secret key;
    payment_code code;
    
    secret(payment_code_version version, bool bitmessage_notification, const hd_secret& pk);
    secret(payment_code_version version, const hd_secret& pk);
    secret():code(low::null_payment_code){}
    
    bool valid() const;
    
    const notification_key notification_address(address_format format) const;
    
    // write the nth pubkey to a given payment code. This operation can fail rarely. 
    bool to(ec_compressed& pubkey, uint32_t n, const payment_code& pc) const;
    
    bool from(ec_compressed& pubkey, uint32_t n, const payment_code& pc) const;

    // Derive address n to another payment code. It is possible that address n doesn't exist, so 
    // increment n if not. The value of n will be the next value that should be used the next
    // time next_address is called. next_address will always return a valid address. 
    const ec_compressed next_derived_pubkey(uint32_t& n, const payment_code& pc) const;
    
    const secret ephemeral(uint32_t n) const;
    
    // TODO cold storage. 
    
private:
    secret(libbitcoin::wallet::hd_private hd);
};
    
inline secret::secret(payment_code_version version, bool bitmessage_notification, const hd_secret& pk)
        :key(pk), code(version, bitmessage_notification, pk.Pubkey) {};
inline secret::secret(payment_code_version version, const hd_secret& pk):secret(version, false, pk){}
    
inline bool secret::valid() const {
    return key.valid() && code.valid();
}

inline const notification_key secret::notification_address(address_format format) const {
    return {key, code.notification_address(format)};
}

namespace low
{
    libbitcoin::wallet::hd_private to_hd_private(secret s);
}

inline const secret secret::ephemeral(uint32_t n) const {
    return secret(low::to_hd_private(*this).derive_private(n));
}

} // bip47

#endif
