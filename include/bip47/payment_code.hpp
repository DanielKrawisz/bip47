#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/wallet/hd_public.hpp>
#include <bitcoin/bitcoin/wallet/ec_private.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>

namespace bip47
{
    
typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::wallet::hd_public hd_public;
typedef libbitcoin::chain::output_point outpoint;
typedef libbitcoin::wallet::ec_private ec_private;

const int payment_code_size = 80;

struct payment_code {
public:
    payment_code(const libbitcoin::byte_array<payment_code_size> code);
    payment_code(uint8_t version, hd_public pubkey, bool bitmessage_notification);
    
    bool valid() const;

    uint8_t version() const;

    bool bitmessage_notification() const;

    hd_public pubkey() const;

    hd_public address(const payment_code& to, unsigned int n) const;
    
    const payment_code mask(const ec_private& pk, const ec_compressed& point, const outpoint& outpoint) const;

    std::string base58_encode(const payment_code& code) const;

    static payment_code base58_decode(std::string);
    
    uint8_t operator[] (const int index) const;
private:
    void mask_payment_code(libbitcoin::long_hash);
    libbitcoin::byte_array<payment_code_size> code;
};

} // bip47

#endif
