#include <array>
#include <bip47/payment_code.hpp>
#include <bip47/notification.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>

using namespace libbitcoin;

namespace bip47
{
    
payment_code::payment_code(const libbitcoin::byte_array<payment_code_size> code):code(code) {}

payment_code::payment_code(payment_code_version version, const ec_compressed& point, const hd_chain_code& chain_code, bool bitmessage_notification) {
    if (version < 1 && version > 3) return;
    code[0] = version;
    code[1] = bitmessage_notification;
    std::copy(std::begin(point), std::end(point), code.at(2));
    std::copy(std::begin(chain_code), std::end(chain_code), code.at(2));
    for (int i = 67; i < payment_code_size; i ++) {
        code[i] = 0;
    }
}

payment_code::payment_code(const data_chunk data) {
    if (data.size() != 80) payment_code();
    std::copy(data.begin(), data.end(), code.begin());
}

uint8_t inline payment_code::operator[] (int index) const {
    return code[index];
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
    std::copy(code.at(2), code.at(36), k.begin());
    return k;
}

const hd_chain_code payment_code::chain_code() const {
    hd_chain_code k;
    std::copy(code.at(36), code.at(68), k.begin());
    return k;
}

// TODO
libbitcoin::wallet::hd_public to_hd_public(const ec_compressed& point, const hd_chain_code& chain_code) {
    return libbitcoin::wallet::hd_public();
}

const address inline payment_code::notification_address(address_format format) const {
    return libbitcoin::wallet::ec_public(to_hd_public(point(), chain_code()).derive_public(0).point()).to_payment_address(format);
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
const libbitcoin::long_hash payment_code_mask(const ec_private& pk, const ec_compressed& point, const outpoint& outpoint) {
    return libbitcoin::null_long_hash;
}

void payment_code::mask_payment_code(libbitcoin::long_hash mask) {
    for (int i = 3; i < 35; i++) code[i] = code[i]; // TODO
    for (int i = 35; i < 67; i++) code[i] = code[i]; // TODO
}

const payment_code payment_code::mask(const ec_private& pk, const ec_compressed& point, const outpoint& outpoint) const {    
    // mask payment code.
    payment_code masked(code);
    masked.mask_payment_code(payment_code_mask(pk, point, outpoint));
    return masked;
}

ec_compressed payment_code::identifier() {
    ec_compressed id;
    auto hash = libbitcoin::sha256_hash(code);
    id[0] = 0x02;
    std::copy(hash.begin(), hash.end(), id.at(1));
    return id;
}

// TODO do I have to add extra digits or anything?
std::string inline payment_code::base58_encode(const payment_code& code) const {
    return libbitcoin::encode_base58(code);
}

bool inline payment_code::base58_decode(payment_code& pc, std::string string) {
    return libbitcoin::decode_base58<payment_code_size>(pc.code, string);
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

}
