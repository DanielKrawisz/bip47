#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{

// TODO
libbitcoin::wallet::hd_private derive_private(const hd_secret& secret, unsigned int n) {
    throw 0;
}

const address secret::next_address(unsigned int& n, const bip47::payment_code& to) const {
    // first, get the zeroth private key. 
    auto pk = derive_private(key, 0);
    while (true) {
        // generate the nth pubkey. 
        auto pubkey = code.pubkey(n);
        // TODO
        n++;
    } 
}

} // bip47
