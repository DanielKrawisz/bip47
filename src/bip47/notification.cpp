#include <bip47/notification.hpp>
#include <bip47/payment_code.hpp>
#include <bip47/versions.hpp>

namespace bip47
{
    
payment_code_version notification::version(const transaction& tx) {
    if (v1::valid_notification(tx)) return 1;
    if (v2::valid_notification(tx)) return 2;
    if (v3::valid_notification(tx)) return 3;
    return 0;
}
    
bool notification::valid(const transaction& tx) {
    return notification::version(tx) != 0;
}

transaction notify_by_version(
    const payment_code_version version, 
    const payment_code& alice, 
    const payment_code& bob,
    const outpoint& to_be_redeemed, 
    const ec_private& designated,
    address_format format,
    const unsigned int amount,
    const std::vector<output> other_outputs)
{
    if (!to_be_redeemed.is_valid()) return transaction();
    if (version == 3) return v3::notify(alice, bob, to_be_redeemed, designated, format, amount, other_outputs);
    if (version == 2) return v2::notify(alice, bob, to_be_redeemed, designated, amount, other_outputs);
    if (version == 1) return v1::notify(alice, bob, to_be_redeemed, designated, format, amount, other_outputs);
    return transaction();
}
    
const transaction notification::notify(
    const payment_code& alice, 
    const payment_code& bob, 
    const transaction& prior, 
    const ec_private& designated,
    address_format format,
    unsigned int amount,
    const transaction::outs other_outputs)
{
    transaction nt = notify_by_version(bob.version(), alice, bob, find_redeemable_output(prior, designated), designated, format, amount, other_outputs);
    if (nt.is_valid()) {
        // TODO sign transaction.
    }

    return nt;
}

notification::notification(const transaction &tx):tx(tx) {}

int notification::version() const {
    return version(tx);
}
    
bool notification::valid() const {
    return valid(tx);
}

bool designated_pubkey_by_version(ec_public &designated, std::vector<transaction> previous, const transaction& tx, payment_code_version version);

bool inline designated_pubkey_by_version(ec_public &designated, std::vector<transaction> previous, const transaction& tx, payment_code_version version) {
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
