#include <array>
#include <bip47/payment_code.hpp>
#include <bip47/notification.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>

namespace bip47
{

payment_code::payment_code(const libbitcoin::byte_array<payment_code_size> code):code(code) {}

payment_code::payment_code(payment_code_version version, const hd_public& pubkey, bool bitmessage_notification) {
    if (version < 1 && version > 3) return;
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
}

payment_code::payment_code(const data_chunk data) {
    if (data.size() != 80) payment_code();
    std::copy(data.begin(), data.end(), code.begin());
}

uint8_t inline payment_code::operator[](int index) const {
    return code[index];
}

bool inline payment_code::operator==(payment_code pc) const {
    return code == pc.code;
}

bool inline payment_code::operator!=(payment_code pc) const {
    return code != pc.code;
}

payment_code_version inline payment_code::version() const {
    return code.at(0);
}

bool inline payment_code::valid() const {
    if (!(code.at(0) > 0 && code.at(0) <= 3 && code.at(1) <= 1 && (code.at(2) == 2 || code.at(2) == 3))) return false;
    for (int i = 67; i < payment_code_size; i ++) if (code.at(i) != 0) return false;
    return true;
}

bool inline payment_code::bitmessage_notification() const {
    return code[1] && 1 == 1;
}

const ec_compressed payment_code::point() const {
    ec_compressed k;
    for (int i = 0; i < k.size(); i ++) {
        k[i] = code[i + 2];
    }
    return k;
}

const hd_chain_code payment_code::chain_code() const {
    hd_chain_code k;
    for (int i = 0; i < k.size(); i++) {
        k[i] = code[i + 36];
    }
    return k;
}

const hd_public inline payment_code::pubkey() const {
    return {point(), chain_code()};
}

libbitcoin::data_slice inline payment_code::slice() const {
    return libbitcoin::data_slice(code);
}

std::string inline payment_code::base58_encode() const {
    auto data = libbitcoin::to_chunk(code);
    libbitcoin::append_checksum(data);
    return libbitcoin::encode_base58(data);
}

bool inline payment_code::base58_decode(payment_code& pc, std::string string) {
    bool success = libbitcoin::decode_base58<payment_code_size>(pc.code, string);
    if (!success) return false;
    auto data = libbitcoin::to_chunk(pc.code);
    if (libbitcoin::verify_checksum(data)) {
        pc = payment_code(data);
        return true;
    }
    return false;
}

inline payment_code::payment_code() {}

void inline payment_code::invalidate() {
    code[0] = 0;
}

const payment_code inline payment_code::base58_decode(std::string string) {
    payment_code pc;
    // Ensure code is invalid if we cannot decode from base 58.
    if (!base58_decode(pc, string)) pc.invalidate();
    return pc;
}

// TODO
libbitcoin::wallet::hd_public to_hd_public(const hd_public& pubkey) {
    return libbitcoin::wallet::hd_public();
}

const address inline payment_code::notification_address(address_format format) const {
    return libbitcoin::wallet::ec_public(to_hd_public(pubkey()).derive_public(0).point()).to_payment_address(format);
}

// TODO
const hd_public payment_code::pubkey(unsigned int n) const {
    return {};
}

// TODO
const hd_public payment_code::change(unsigned int n) const {
    return {};
}

// TODO
const libbitcoin::long_hash payment_code_mask(const ec_secret& pk, const ec_compressed& point, const outpoint& outpoint) {
    return libbitcoin::null_long_hash;
}

void payment_code::mask_payment_code(libbitcoin::long_hash mask) {
    for (int i = 3; i < 35; i++) code[i] = code[i]; // TODO
    for (int i = 35; i < 67; i++) code[i] = code[i]; // TODO
}

const payment_code payment_code::mask(const ec_secret& pk, const ec_compressed& point, const outpoint& outpoint) const {    
    // mask payment code.
    payment_code masked(code);
    masked.mask_payment_code(payment_code_mask(pk, point, outpoint));
    return masked;
}

const payment_code_identifier payment_code::identifier() const {
    payment_code_identifier id;
    auto hash = libbitcoin::sha256_hash(code);
    id[0] = 0x02;
    for (int i = 0; i < hash.size(); i ++) {
        id[i + 1] = hash[i];
    }
    return id;
}

} // bip47
