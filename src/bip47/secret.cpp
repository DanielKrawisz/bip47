#include <bip47/secret.hpp>
#include <bip47/notification.hpp>
#include <abstractions/hd/bip32.hpp>

namespace bip47
{
    
bool secret::to(ec_compressed& pubkey, uint32_t n, const payment_code& pc) const {
    ec_compressed B = pc.derive_pubkey(n);
    
    ec_compressed secret_point = B * key.Secret;
    if (!secret_point.valid()) return false;
    
    ec_secret s = libbitcoin::sha256_hash(secret_point);
    
    if (!s.valid()) return false;
    
    pubkey = B + low::point(pc) * s;
    return true;
}

const ec_compressed secret::next_derived_pubkey(uint32_t& n, const bip47::payment_code& pc) const {
    ec_compressed pubkey;
    do n++; while (!to(pubkey, n - 1, pc));
    return pubkey;
}

} // bip47
