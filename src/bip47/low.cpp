#include <bip47/low.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace bip47
{

namespace low
{

bool payload(payment_code& out, const output& output) {
    const auto ops = output.script().operations();
    if (!libbitcoin::chain::script::is_pay_null_data_pattern(ops)) return false;
    return from_data(out, ops[1].data());
}

bool identifier_equals(const ec_compressed &x, const data_chunk& y) {
    if (y.size() != libbitcoin::ec_compressed_size) return false;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) if (x[i] != y[i]) return false;
    return true;
}

const ec_compressed to_public(const ec_secret& key) {
    ec_compressed point;
    libbitcoin::secret_to_public(point, key);
    return point;
}

const hd_chain_code chain_code(const libbitcoin::wallet::hd_key& hd) {
    hd_chain_code cc;
    for (int i = 0; i < libbitcoin::wallet::hd_chain_code_size; i++) {
        cc[i] = hd[13 + i];
    }
    
    return cc;
}

const ec_compressed public_key(const libbitcoin::wallet::hd_key& hd) {
    ec_compressed ec;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        ec[i] = hd[13 + libbitcoin::wallet::hd_chain_code_size + i];
    }
    
    return ec;
}

const ec_secret secret_key(const libbitcoin::wallet::hd_key& hd) {
    ec_secret k;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        k[i] = hd[14 + libbitcoin::wallet::hd_chain_code_size + i];
    }
    
    return k;
}

const libbitcoin::wallet::hd_key to_public(const libbitcoin::wallet::hd_key hd) {
    libbitcoin::wallet::hd_key k(hd);
    ec_secret p;
    for (int i = 0; i < libbitcoin::ec_secret_size; i++) {
        p[i] = hd[13 + libbitcoin::wallet::hd_chain_code_size + 1 + i];
    }
    ec_compressed pubkey = low::to_public(p);
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) {
        k[13 + libbitcoin::wallet::hd_chain_code_size + i] = pubkey[i];
    }
    
    return k;
}

bool to(const output& output, const address& notification_address) {
    const auto ops = output.script().operations();
    return (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == notification_address);
}

bool read_notification_payload(payment_code& pc, const output& output) {
    const auto ops = output.script().operations();
    if (!libbitcoin::chain::script::is_pay_null_data_pattern(ops) && ops[1].data().size() == payment_code_size) return false;
    pc = bip47::payment_code(output.script().operations()[1].data());
    if (!pc.valid) return false;
}

namespace v1
{

bool is_notification_change_output_pattern(const libbitcoin::machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 2 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 2) return false;
    if (ops[1].data().size() != libbitcoin::ec_compressed_size) return false;
    if (ops[2].data().size() != libbitcoin::ec_compressed_size) return false;
    
    return true;
}

} // v1

namespace v2
{

const output inline notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount) {
    // TODO randomization/ordering policy? 
    payment_code_identifier bob_id;
    identifier(bob_id, bob);
    return output(amount, libbitcoin::chain::script::to_pay_multisig_pattern(1, {alice, bob_id}));
}

const bool is_notification_change_output_to(const output& output, const payment_code_identifier& bob_id) {
    if (!is_notification_change_output(output)) return false;
    identifier_equals(bob_id, output.script().operations()[1].data());
    return true;
}

} // v2

} // low

} // bip47
