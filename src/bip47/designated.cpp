#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bip47/patterns.hpp>

namespace bip47
{
    
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::wallet::ec_public ec_public;
typedef libbitcoin::data_chunk data_chunk;

bool extract_designated_pubkey(
    ec_public& out,
    const libbitcoin::machine::operation::list& input,
    const libbitcoin::machine::operation::list& prevout_script)
{
    if (libbitcoin::chain::script::is_pay_public_key_pattern(prevout_script)) {
        if (libbitcoin::chain::script::is_sign_public_key_pattern(input)) {
            out = prevout_script[0].data();
            return true;
        }

        return false;
    }

    if (libbitcoin::chain::script::is_pay_key_hash_pattern(prevout_script)) {
        if (libbitcoin::chain::script::is_sign_key_hash_pattern(input)) {
            out = input[1].data();
            return true;
        }

        return false;
    }

    if (libbitcoin::chain::script::is_pay_multisig_pattern(prevout_script)) {
        if (libbitcoin::chain::script::is_sign_multisig_pattern(input)) {
            out = prevout_script[1].data();
            return true;
        }

        return false;
    }

    if (!libbitcoin::chain::script::is_pay_script_hash_pattern(prevout_script)) {
        return false;
    }

    if (libbitcoin::chain::is_public_key_pattern(input)) {
        out = input[1].data();
        return true;
    }

    if (libbitcoin::chain::is_key_hash_pattern(input)) {
        out = input[1].data();
        return true;
    }

    unsigned int middle;
    if (libbitcoin::chain::is_multisig_script_hash_pattern(middle, input)) {
        out = input[middle + 1].data();
        return true;
    }

    return false;
}

}
