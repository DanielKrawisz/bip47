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
        const ec_secret& designated, 
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
        auto ops = output.script().operations();
        chain::script::is_pay_null_data_pattern(ops);
        if (ops[1].data().size() == 80) return true;
    }
    
    return false;
}

bool v1::designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt) {
    return designated_pubkey(out, previous, nt);
}

bool v2::designated_pubkey(data_chunk& out, const std::vector<transaction>& previous, const transaction& nt) {
    return designated_pubkey(out, previous, nt);
}

}
