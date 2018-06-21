#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{
    
bool secret::to(ec_compressed& pubkey, uint32_t n, const payment_code& pc) const {
    ec_compressed B = pc.derive_pubkey(n);
    ec_compressed secret_point = B;
    
    if (!libbitcoin::ec_multiply(secret_point, key.key)) return false;
    
    ec_secret s = libbitcoin::sha256_hash(secret_point);
    
    if (!low::is_secp256k1(s)) return false;
    
    pubkey = low::point(pc);
    if (!libbitcoin::ec_multiply(pubkey, s)) return false;
    
    // TODO finish this. I don't know how to add public keys together. 
    return true;
}

const ec_compressed secret::next_derived_pubkey(uint32_t& n, const bip47::payment_code& pc) const {
    ec_compressed pubkey;
    do n++; while (!to(pubkey, n - 1, pc));
    return pubkey;
}

} // bip47
