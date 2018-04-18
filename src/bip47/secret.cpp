#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{

secret::secret(uint8_t version, const ec_private& pk, const hd_chain_code& chain_code, bool bitmessage_notification)
    :key(pk), code(bip47::payment_code(version, pk.to_public(), chain_code, bitmessage_notification)) {}

// TODO
libbitcoin::wallet::hd_private derive_private(const ec_private& secret, const hd_chain_code chain_code, unsigned int n);

const payment_code& secret::pubcode() {
    return code;
}

const address secret::next_address(unsigned int& n, const bip47::payment_code& to) const {
    // first, get the zeroth private key. 
    auto pk = derive_private(key, code.chain_code(), 0);
    while (true) {
        // generate the nth pubkey. 
        auto pubkey = code.pubkey(n);
        // TODO
        n++;
    } 
}
    
} // bip47
