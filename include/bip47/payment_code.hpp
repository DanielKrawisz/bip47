#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bip47/hd.hpp>

namespace bip47
{
    
typedef libbitcoin::chain::output_point outpoint;
typedef libbitcoin::wallet::payment_address address;
typedef uint8_t payment_code_version;
typedef uint8_t address_format;
typedef ec_compressed payment_code_identifier;

const int payment_code_size = 80;

struct payment_code {
    payment_code(const libbitcoin::byte_array<payment_code_size> code);
    payment_code(payment_code_version version, const hd_public& pubkey, bool bitmessage_notification);
    payment_code(const data_chunk data);
    
    bool valid() const;

    payment_code_version version() const;

    bool bitmessage_notification() const;

    const ec_compressed point() const;
    const hd_chain_code chain_code() const;
    const hd_public     pubkey() const;
    
    const address notification_address(address_format format) const;
    
    const hd_public pubkey(unsigned int n) const;
    
    const hd_public change(unsigned int n) const;
    
    const payment_code mask(const ec_secret&, const ec_compressed& point, const outpoint& outpoint) const;
    
    const payment_code_identifier identifier() const;

    std::string base58_encode() const;

    static const payment_code base58_decode(std::string string);
    
    uint8_t operator[](int) const;
    bool operator==(payment_code) const;
    bool operator!=(payment_code) const;
    
    libbitcoin::data_slice slice() const;
private:
    libbitcoin::byte_array<payment_code_size> code;
    
    payment_code();
    void invalidate();
    void mask_payment_code(libbitcoin::long_hash);
    static bool base58_decode(payment_code& code, std::string string);
};

} // bip47

#endif
