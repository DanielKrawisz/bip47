#ifndef BIP47_HD_HPP
#define BIP47_HD_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/hd_public.hpp>
#include <abstractions/hd/bip32.hpp>

namespace bip47
{
    
typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::ec_secret ec_secret;
typedef libbitcoin::wallet::hd_chain_code hd_chain_code;
typedef libbitcoin::data_chunk data_chunk;

typedef abstractions::hd::secp256k1::public_node hd_public;
typedef abstractions::hd::secp256k1::private_node hd_secret;

namespace hd
{
    
    const data_chunk data(const hd_public&);
    const data_chunk data(const hd_secret&);
    
    bool from_data(const data_chunk, hd_public&);
    bool from_data(const data_chunk, hd_secret&);
    
    bool from_hd_key(const libbitcoin::wallet::hd_key& hd, hd_public&);
    bool from_hd_key(const libbitcoin::wallet::hd_key& hd, hd_secret&);
    
}

// low contains low-level functions. 
namespace low
{

const ec_compressed to_public(const ec_secret& key);

const hd_chain_code chain_code(const libbitcoin::wallet::hd_key& hd);

const ec_compressed public_key(const libbitcoin::wallet::hd_key& hd);

const ec_secret secret_key(const libbitcoin::wallet::hd_key& hd);

} // low

namespace hd
{

inline bool from_hd_key(const libbitcoin::wallet::hd_key& key, hd_public& h) {
    h = hd_public(low::public_key(key), low::chain_code(key));
    return h.valid();
}

inline bool from_hd_key(const libbitcoin::wallet::hd_key& key, hd_secret& h) {
    ec_secret s = low::secret_key(key);
    h = hd_secret(s, low::to_public(s), low::chain_code(key));
    return h.valid();
}

} // hd

} // bip47

#endif
