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
typedef libbitcoin::wallet::hd_chain_code hd_chain_code;
typedef libbitcoin::chain::output_point outpoint;
typedef libbitcoin::wallet::ec_private ec_private;
typedef libbitcoin::wallet::payment_address address;
typedef libbitcoin::data_chunk data_chunk;
typedef uint8_t payment_code_version;
typedef uint8_t address_format;

const int payment_code_size = 80;

struct hd_public {
    const ec_compressed point;
    const hd_chain_code chain_code;
    
    data_chunk data() const;
};

struct payment_code {
    payment_code(const libbitcoin::byte_array<payment_code_size> code);
    payment_code(payment_code_version version, const ec_compressed& point, const hd_chain_code& chain_code, bool bitmessage_notification);
    payment_code(const data_chunk data);
    
    bool valid() const;

    payment_code_version version() const;

    bool bitmessage_notification() const;

    const ec_compressed point() const;
    const hd_chain_code chain_code() const;
    const hd_public     hd_public_key() const;
    
    const address notification_address(address_format format) const;
    
    const hd_public pubkey(unsigned int n) const;
    
    const hd_public change(unsigned int n) const;
    
    const payment_code mask(const ec_private&, const ec_compressed& point, const outpoint& outpoint) const;
    
    const ec_compressed identifier() const;

    std::string base58_encode(const payment_code&) const;

    static const payment_code base58_decode(std::string string);
    
    uint8_t operator[] (int) const;
private:
    libbitcoin::byte_array<payment_code_size> code;
    
    payment_code();
    void invalidate();
    void mask_payment_code(libbitcoin::long_hash);
    static bool base58_decode(payment_code& code, std::string string);
};

} // bip47

#endif
