#include <bip47/payment_code.hpp>
#include <bip47/notification.hpp>
#include <bip47/low.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>
#include <array>


namespace bip47
{
    
namespace low
{

const payment_code new_payment_code(payment_code_version version, bool bitmessage_notification, const hd_public& pubkey) {
    payment_code code;
    code[0] = version;
    code[1] = bitmessage_notification;
    
    int n = 2;
    for (auto p : pubkey.point) {
        code[n] = p;
        n++;
    }
    
    for (auto p : pubkey.chain_code) {
        code[n] = p;
        n++;
    }
    
    for (; n < payment_code_size; n++) {
        code[n] = 0;
    }
    
    return payment_code(code);
}

bool valid(const payment_code& code) {
    // check version. 
    if (code[0] == 0 || code[0] > 3) return false;
    
    // check valid flags. 
    if (code[1] > bitmessage_notification_flag) return false;
    
    // check sign byte.  
    if (code[2] != 2 && code[2] != 3) return false;
    
    // check that the end is zero. 
    for (int i = 67; i < payment_code_size; i ++) if (code[i] != 0) return false;
    return true;
}

bool from_data(payment_code& code, const data data) {
    if (data.size() != 80) return false;
    std::copy(data.begin(), data.end(), code.begin());
    return true;
}

const payment_code payment_code_from_data(const data data) {
    payment_code code;
    from_data(code, data);
    return code;
}

const data encode_with_checksum(const payment_code& code) {
    auto data = libbitcoin::to_chunk(code);
    libbitcoin::append_checksum(data);
    return data;
}

const int checksum_size = 4;

const int payment_code_with_checksum_size = payment_code_size + checksum_size;

bool base58_decode(payment_code& code, std::string string) {
    libbitcoin::byte_array<payment_code_with_checksum_size> with_checksum;
    bool success = libbitcoin::decode_base58<payment_code_with_checksum_size>(with_checksum, string);
    if (!success) return false;
    
    if (!libbitcoin::verify_checksum(with_checksum)) {
        return false;
    }
    
    for (int i = 0; i < payment_code_size; i++) {
        code[i] = with_checksum[i];
    }
    
    return true;
}

const ec_compressed point(const payment_code& code) {
    ec_compressed k;
    for (int i = 0; i < k.size(); i ++) {
        k[i] = code[i + 2];
    }
    return k;
}

const hd_chain_code chain_code(const payment_code& code) {
    hd_chain_code k;
    for (int i = 0; i < k.size(); i++) {
        k[i] = code[i + 35];
    }
    return k;
}

inline void invalidate(payment_code& code) {
    code[0] = 0;
}

// TODO
const libbitcoin::wallet::hd_public to_hd_public(const payment_code& code) {
    throw 0;
}

void payment_code_identifier(ec_compressed& identifier, const payment_code& code) {
    auto hash = libbitcoin::sha256_hash(code);
    identifier[0] = 0x02;
    for (int i = 0; i < hash.size(); i ++) {
        identifier[i + 1] = hash[i];
    }
}

} // low

const payment_code payment_code::base58_decode(std::string string) {
    low::payment_code code;
    // Ensure code is invalid if we cannot decode from base 58.
    if (!low::base58_decode(code, string)) low::invalidate(code);
    return code;
}

const payment_code_identifier payment_code::identifier() const {
    ec_compressed id;
    low::payment_code_identifier(id, *this);
    return id;
}

// TODO
const hd_public payment_code::pubkey(unsigned int n) const {
    return {};
}

// TODO
const hd_public payment_code::change(unsigned int n) const {
    return {};
}

} // bip47
