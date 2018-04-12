#include <bip47/versions.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include "designated.cpp"

namespace bip47
{
    
typedef libbitcoin::wallet::payment_address payment_address;

const transaction v1::notify(
        const payment_code& from, 
        const ec_compressed& to, 
        const outpoint& prior, 
        const ec_private& designated, 
        unsigned int amount)
{
    // TODO insert input for signing. 
    return libbitcoin::chain::transaction(1, 0, {}, {
        libbitcoin::chain::output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(payment_address(to, payment_address::mainnet_p2sh)))), 
        chain::output(0, chain::script(libbitcoin::chain::script::to_pay_null_data_pattern(from.mask(designated, to, prior))))});
}

bool v1::valid(const transaction& tx)
{
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (!chain::script::is_pay_null_data_pattern(ops)) continue;
        if (ops[1].data().size() == 80) return true;
    }
    
    return false;
}

bool v1::designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt) {
    return designated_pubkey(out, previous, nt);
}

bool is_bip47_v2_multisig_pattern(const machine::operation::list& ops);

bool is_bip47_v2_multisig_pattern(const machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 2 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 2) return false;
    
    // TODO I don't know if anything else goes in here. 
    
    return true;
}

bool v2::valid(const transaction& tx)
{
    if (!v1::valid(tx)) return false;
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (!is_bip47_v2_multisig_pattern(ops)) continue;
        else return true;
    }
    
    return false;
}

bool v2::designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt) {
    return designated_pubkey(out, previous, nt);
}

bool is_bip47_v3_multisig_pattern(const machine::operation::list& ops);

bool is_bip47_v3_multisig_pattern(const machine::operation::list& ops) {
    if (libbitcoin::chain::script::is_pay_multisig_pattern(ops)) {
        return false;
    }
    
    // Are we talking about a 1 of 3 multisig? 
    if (static_cast<uint8_t>(ops[0].code()) != 1) return false;
    if (static_cast<uint8_t>(ops[ops.size() - 2].code()) != 3) return false;
    
    // Are the first two keys compressed? 
    if (ops[1].data().size() != ec_compressed_size) return false;
    if (ops[1].data().size() != ec_compressed_size) return false;
    
    // Is the last key uncompressed? 
    if (ops[1].data().size() != ec_uncompressed_size) return false;
    
    return true;
}

bool v3::valid(const transaction& tx)
{
    for (auto output : tx.outputs()) {
        const auto ops = output.script().operations();
        if (!is_bip47_v3_multisig_pattern(ops)) return false;
    }
    
    return true;
}

}
