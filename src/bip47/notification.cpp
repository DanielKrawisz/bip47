#include <bip47/notification.hpp>
#include <bip47/payment_code.hpp>
#include <bip47/versions.hpp>

namespace bip47
{
    
uint8_t notification::version(const transaction& tx) {
    if (v1::valid(tx)) return 1;
    if (v2::valid(tx)) return 2;
    if (v3::valid(tx)) return 3;
    return 0;
}
    
bool notification::valid(const transaction& tx) {
    return notification::version(tx) != 0;
}

transaction notify_by_version(
    const uint8_t version, 
    const payment_code& from, 
    const hd_public& to,
    const outpoint& to_be_redeemed, 
    const ec_private& designated,
    const unsigned int amount)
{
    if (!to_be_redeemed.is_valid()) return transaction();
    if (version == 3) return v3::notify(from, to, to_be_redeemed, designated, amount);
    if (version == 2) return v2::notify(from, to, to_be_redeemed, designated, amount);
    if (version == 1) return v1::notify(from, to, to_be_redeemed, designated, amount);
    return transaction();
}
    
const transaction notification::notify(
        const payment_code& from, 
        const payment_code& to, 
        const transaction& prior, 
        const ec_private& designated,
        unsigned int amount)
{
    transaction nt = notify_by_version(to.version(), from, to.pubkey(), find_redeemable_output(prior, designated), designated, amount);
    if (nt.is_valid()) {
        // TODO sign transaction.
    }

    return nt;
}

notification::notification(const chain::transaction &tx):tx(tx) {}

int notification::version() const {
    return version(tx);
}
    
bool notification::valid() const {
    return valid(tx);
}

bool designated_pubkey_by_version(ec_public &designated, std::vector<transaction> previous, const transaction& tx, uint8_t version);

bool inline designated_pubkey_by_version(ec_public &designated, std::vector<transaction> previous, const transaction& tx, int version) {
    if (version == 1) return v1::designated_pubkey(designated, previous, tx);
    if (version == 2) return v2::designated_pubkey(designated, previous, tx);
    return false;
}
    
bool notification::designated_pubkey(ec_public &designated, std::vector<transaction> previous, const transaction& tx) {
    return designated_pubkey_by_version(designated, previous, tx, notification::version(tx));
}
    
bool notification::designated_pubkey(ec_public &designated, std::vector<transaction> previous) const {
    return designated_pubkey_by_version(designated, previous, tx, notification::version(tx));
}

//TODO should we check for anything other than pay to pubkey and pay to pubkey hash?
const outpoint notification::find_redeemable_output(const transaction& tx, const ec_private& pk) {
    const auto pubkey = pk.to_public();
    const address address = pk.to_payment_address();
    for (auto o : tx.outputs()) {
        const auto ops = o.script().operations();
        if (libbitcoin::chain::script::is_pay_key_hash_pattern(ops)) {
            // TODO Is the address one we can redeem? 
        } else if (libbitcoin::chain::script::is_pay_public_key_pattern(ops)) {
            // TODO Is the public key one we can redeem?
        }
    }
}

}
