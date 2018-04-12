#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/hd_public.hpp>
#include <bitcoin/bitcoin/wallet/ec_private.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace bip47
{
    
typedef libbitcoin::ec_compressed ec_compressed;
typedef libbitcoin::wallet::hd_public hd_public;
typedef libbitcoin::chain::output_point outpoint;
typedef libbitcoin::wallet::ec_private ec_private;
typedef libbitcoin::wallet::payment_address address;
typedef libbitcoin::data_chunk data_chunk;

const int payment_code_size = 80;

struct payment_code {
public:
    payment_code(const libbitcoin::byte_array<payment_code_size> code);
    payment_code(uint8_t version, hd_public pubkey, bool bitmessage_notification);
    payment_code(const data_chunk data);
    
    bool valid() const;

    uint8_t version() const;

    bool bitmessage_notification() const;

    hd_public pubkey() const;
    
    address address(uint8_t version) const;

    hd_public address_to(const payment_code& to, unsigned int n) const;
    
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
