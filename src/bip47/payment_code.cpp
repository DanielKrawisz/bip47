#include <array>
#include <bip47/payment_code.hpp>
#include <bip47/notification.hpp>

using namespace libbitcoin;

namespace bip47
{
    
payment_code::payment_code(const libbitcoin::byte_array<payment_code_size> code):code(code) {}

// TODO
payment_code::payment_code(uint8_t version, hd_public pubkey, bool bitmessage_notification) {
    if (version < 1 && version > 3) return;
    code[0] = version;
    code[1] = bitmessage_notification;
    std::copy(pubkey.point().begin(), pubkey.point().end(), code.at(2));
    std::copy(pubkey.chain_code().begin(), pubkey.chain_code().end(), code.at(2));
    for (int i = 67; i < payment_code_size; i ++) {
        code[i] = 0;
    }
}

uint8_t inline payment_code::operator[] (const int index) const {
    return code[index];
}

uint8_t inline payment_code::version() const {
    return code.at(0);
}

bool inline payment_code::valid() const {
    if (!(code.at(0) > 0 && code.at(0) <= 3 && code.at(1) <= 1 && (code.at(2) == 2 || code.at(2) == 3))) return false;
    for (int i = 67; i < payment_code_size; i ++) if (code.at(i) != 0) return false;
    return true;
}

bool payment_code::bitmessage_notification() const {
    return code[1] && 1 == 1;
}

// TODO
/*hd_public pubkey() const;

hd_public address(const payment_code& to, unsigned int n) const;
*/

// TODO
const libbitcoin::long_hash payment_code_mask(const ec_private& pk, const ec_compressed& point, const outpoint& outpoint); 

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

/*std::string payment_code::base58_check_encode(const payment_code& code) {
}

payment_code payment_code::base58_check_decode(std::string string) {
    
}*/

}
