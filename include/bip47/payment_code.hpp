#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <string>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bip47/hd.hpp>

namespace bip47
{

typedef libbitcoin::data_chunk data;
typedef uint8_t payment_code_version;
typedef uint8_t address_format;
typedef libbitcoin::wallet::payment_address address;
typedef libbitcoin::chain::output_point outpoint;
typedef ec_compressed payment_code_identifier;

const int payment_code_size = 80;
const uint8_t bitmessage_notification_flag = 1;// low contains low-level functions. 

// namespace low is for low-level functions and constructs.
namespace low
{

typedef libbitcoin::byte_array<payment_code_size> payment_code;

} // low

struct payment_code : public low::payment_code {
                       bool valid() const;
       payment_code_version version() const;
                       bool bitmessage_notification() const;
        const ec_compressed point() const;
        const hd_chain_code chain_code() const;
            const hd_public pubkey() const;
              const address notification_address(address_format format) const;
        const ec_compressed derive_pubkey(uint32_t n) const;
    payment_code_identifier identifier() const;
          const std::string base58() const;
    
    // constructors
    payment_code();
    payment_code(const low::payment_code code);
    payment_code(payment_code_version version, bool bitmessage_notification, const hd_public& pubkey);
    payment_code(payment_code_version version, const hd_public& pubkey);
    payment_code(const data_chunk data);

    static const payment_code base58_decode(std::string string);
};

// low contains low-level functions. 
namespace low
{

const payment_code null_payment_code{
    0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const payment_code new_payment_code(payment_code_version version, bool bitmessage_notification, const hd_public& pubkey);

bool valid(const payment_code& code);

bool from_data(payment_code& code, const data data);

const payment_code payment_code_from_data(const data data);

inline const data encode(const payment_code& code) {
    return libbitcoin::to_chunk(code);
}

const data encode_with_checksum(const payment_code& code);
    
bool decode_base_58(payment_code& code, const std::string string);

inline payment_code_version version(const payment_code& code) {
    return code[0];
}

inline uint8_t flags(const payment_code& code) {
    return code[1];
}

inline bool bitmessage_notification(const payment_code& code) {
    return (flags(code) & bitmessage_notification_flag) == bitmessage_notification_flag;
}

const ec_compressed point(const payment_code& code);

const hd_chain_code chain_code(const payment_code& code);

const libbitcoin::wallet::hd_public to_hd_public(const payment_code& code);

inline const address notification_address(const payment_code& code, address_format format) {
    return libbitcoin::wallet::ec_public(point(code)).to_payment_address(format);
}

const void identifier(payment_code_identifier& id, const payment_code& code);

const payment_code mask_payment_code(const payment_code& code, const ec_secret& designated, const ec_compressed& point, const outpoint& outpoint);

} // low

inline bool payment_code::valid() const {
    return low::valid(*this);
}

inline payment_code_version payment_code::version() const {
    return low::version(*this);
}

inline bool payment_code::bitmessage_notification() const {
    return low::bitmessage_notification(*this);
}

inline const ec_compressed payment_code::point() const {
    return low::point(*this);
}

inline const hd_chain_code payment_code::chain_code() const {
    return low::chain_code(*this);
}

inline const hd_public payment_code::pubkey() const {
    return {point(), chain_code()};
}

inline const ec_compressed payment_code::derive_pubkey(uint32_t n) const {
    return low::to_hd_public(*this).derive_public(n).point();
}

inline const std::string payment_code::base58() const {
    return libbitcoin::encode_base58(low::encode_with_checksum(*this));
}

inline const address payment_code::notification_address(address_format format) const {
    return low::notification_address(*this, format);
}

inline payment_code::payment_code():low::payment_code(low::null_payment_code){}
inline payment_code::payment_code(const low::payment_code code):low::payment_code(code) {}
inline payment_code::payment_code(payment_code_version version, bool bitmessage_notification, const hd_public& pubkey)
        :low::payment_code(low::new_payment_code(version, bitmessage_notification, pubkey)){}
inline payment_code::payment_code(payment_code_version version, const hd_public& pubkey):payment_code(version, false, pubkey){}

} // bip47

#endif
