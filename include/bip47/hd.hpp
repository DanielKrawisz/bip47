#ifndef BIP47_HD_HPP
#define BIP47_HD_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/hd_public.hpp>

namespace bip47
{
    
typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::ec_secret ec_secret;
typedef libbitcoin::wallet::hd_chain_code hd_chain_code;
typedef libbitcoin::data_chunk data_chunk;

struct hd_public {
    ec_compressed point;
    hd_chain_code chain_code;
    
    hd_public();
    hd_public(ec_compressed point, hd_chain_code chain_code);
    hd_public(libbitcoin::wallet::hd_key hd);
    
    bool valid() const;
    
    const data_chunk data() const;
    
    bool operator==(const hd_public code) const;
    bool operator!=(const hd_public code) const;
    
    static const hd_public from_data(data_chunk);
};

struct hd_secret {
    ec_secret key;
    hd_chain_code chain_code;

    hd_secret();
    hd_secret(ec_secret key, hd_chain_code chain_code);
    hd_secret(libbitcoin::wallet::hd_key hd);
    
    bool valid() const;
    
    const data_chunk data() const;
    
    const hd_public pubkey() const;
    
    bool operator==(const hd_secret code) const;
    bool operator!=(const hd_secret code) const;
    
    static const hd_secret from_data(data_chunk);
};

const ec_secret null_ec_secret = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const ec_secret null_hd_chain_code = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// low contains low-level functions. 
namespace low
{
    
const ec_compressed to_public(const ec_secret& key);
    
const hd_chain_code chain_code(const libbitcoin::wallet::hd_key& hd);
    
const ec_compressed public_key(const libbitcoin::wallet::hd_key& hd);

const ec_secret secret_key(const libbitcoin::wallet::hd_key& hd);
    
const libbitcoin::wallet::hd_key to_public(const libbitcoin::wallet::hd_key);

// TODO is this right? 
// Also need to check whether it's below a certain number. 
inline bool is_secp256k1(ec_secret pubkey) {
    return pubkey != null_ec_secret;
}

} // low
    
inline hd_public::hd_public():point(libbitcoin::null_compressed_point), chain_code(null_hd_chain_code){}
inline hd_public::hd_public(ec_compressed point, hd_chain_code chain_code):point(point), chain_code(chain_code) {}
inline hd_public::hd_public(libbitcoin::wallet::hd_key hd)
        :point(low::public_key(hd)), chain_code(low::chain_code(hd)){}

inline bool hd_public::valid() const {
    return point != libbitcoin::null_compressed_point;
}

inline bool hd_public::operator== (const hd_public code) const {
    return point == code.point && chain_code == code.chain_code;
}

inline bool hd_public::operator!= (const hd_public code) const {
    return point != code.point || chain_code != code.chain_code;
}

inline const hd_public hd_secret::pubkey() const {
    return hd_public(low::to_public(key), chain_code);
}

inline hd_secret::hd_secret():key(null_ec_secret), chain_code(null_hd_chain_code){};
inline hd_secret::hd_secret(ec_secret key, hd_chain_code chain_code):key(key), chain_code(chain_code) {};
inline hd_secret::hd_secret(libbitcoin::wallet::hd_key hd):key(low::secret_key(hd)), chain_code(low::chain_code(hd)){}

inline bool hd_secret::valid() const {
    return key != null_ec_secret;
}

inline bool hd_secret::operator== (const hd_secret code) const {
    return key == code.key && chain_code == code.chain_code;
}

inline bool hd_secret::operator!= (const hd_secret code) const {
    return key != code.key || chain_code != code.chain_code;
}

} // bip47

#endif
