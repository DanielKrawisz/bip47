#include <array>
#include <bip47/payment_code.hpp>
#include <bip47/notification.hpp>

using namespace libbitcoin;

namespace bip47
{
    
payment_code::payment_code(const libbitcoin::byte_array<payment_code_size> code):code(code) {}

// TODO
payment_code::payment_code(unsigned int version, hd_public pubkey, bool bitmessage_notification) {}

uint8_t payment_code::version() const {
    return code.at(0);
}

bool payment_code::bitmessage_notification() const {
    return code[1] && 1 == 1;
}

// TODO
/*hd_public pubkey() const;

hd_public address(const payment_code& to, unsigned int n) const;
*/

// TODO
const libbitcoin::long_hash payment_code_mask(const ec_secret& pk, const ec_compressed& point, const outpoint& outpoint); 

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

/*std::string payment_code::base58_check_encode(const payment_code& code) {
}

payment_code payment_code::base58_check_decode(std::string string) {
    
}*/

uint8_t payment_code::operator[] (const int index) const {
    return code[index];
}

}
