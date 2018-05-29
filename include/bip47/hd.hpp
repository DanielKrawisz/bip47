#ifndef BIP47_HD_HPP
#define BIP47_HD_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/hd_public.hpp>

namespace bip47
{
    
typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::wallet::hd_chain_code hd_chain_code;
typedef libbitcoin::ec_secret ec_secret;
typedef libbitcoin::data_chunk data_chunk;

struct hd_public {
    const ec_compressed point;
    const hd_chain_code chain_code;
    
    bool valid() const;
    
    data_chunk data() const;
    
    bool operator==(hd_public) const;
    
    static const hd_public from_data(data_chunk);
};

struct hd_secret {
    const ec_secret key;
    const hd_chain_code chain_code;
    
    bool valid() const;
    
    data_chunk data() const;
    
    const hd_public to_public() const;
    bool operator==(hd_secret) const;
    
    static const hd_secret from_data(data_chunk);
};

} // bip47

#endif
