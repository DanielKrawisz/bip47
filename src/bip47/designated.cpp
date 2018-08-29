#ifndef BIP47_DESIGNATED_CPP
#define BIP47_DESIGNATED_CPP

#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bip47/patterns.hpp>
#include <bip47/notification.hpp>

namespace bip47
{
    
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::wallet::ec_public ec_public;
typedef libbitcoin::data_chunk data_chunk;

bool extract_designated_pubkey(
    data_chunk& out,
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

    if (libbitcoin::chain::is_sign_script_hash_pay_public_key_pattern(input)) {
        out = input[1].data();
        return true;
    }

    if (libbitcoin::chain::is_sign_script_hash_pay_key_hash_pattern(input)) {
        out = input[1].data();
        return true;
    }

    unsigned int middle;
    if (libbitcoin::chain::is_sign_script_hash_pay_multisig_pattern(middle, input)) {
        out = input[middle + 1].data();
        return true;
    }

    return false;
}

namespace low
{

bool designated_pubkey_and_outpoint(
    ec_public& designated,
    outpoint& op, 
    const blockchain& b, 
    const transaction& nt)
{
    for (auto in : nt.inputs()) {

        // Get the previous output corresponding to this input.
        output o = b.previous(in.previous_output());
        
        // We need to know that we have found the first input with a
        // designated pubkey in it so if any is null before we've 
        // found it, then we have to return false. 
        if (o == output()) return false;

        data_chunk pubkey;
        if (extract_designated_pubkey(
                pubkey,
                in.script().operations(),
                o.script().operations())) {
            op = in.previous_output();
            designated = ec_public(pubkey);
            return true;
        }
    }

    return false;
}

} // low 

} // bip47 

#endif
