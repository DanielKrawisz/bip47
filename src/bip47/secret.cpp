#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{

secret::secret(payment_code_version version, const hd_secret& pk, bool bitmessage_notification)
    :key(pk), code(version, pk.pubkey(), bitmessage_notification) {}


const bool inline secret::valid() {
    return key.valid() && code.valid();
}

// TODO
libbitcoin::wallet::hd_private derive_private(const hd_secret& secret, unsigned int n);

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
