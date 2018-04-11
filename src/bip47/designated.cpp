#include <bip47/patterns.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>

namespace bip47
{
    
typedef libbitcoin::chain::transaction transaction;
typedef libbitcoin::data_chunk data_chunk;

bool extract_designated_pubkey(
    data_chunk& out,
    const libbitcoin::machine::operation::list& input,
    const libbitcoin::machine::operation::list& prevout_script);

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

    if (!libbitcoin::chain::script::is_pay_script_hash_pattern(
            prevout_script)) {
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

// The previous transactions are not necessarily given in order.
bool designated_pubkey(
    data_chunk& out,
    const std::vector<transaction>& previous, 
    const transaction& nt)
{
    auto inputs = nt.inputs();
    const unsigned int size = inputs.size();
    if (size < 1 || size != previous.size()) {
        return false;
    }

    // A type used to iterate over the previous transactions, which might be out
    // of order.
    class matcher
    {
    public:
        const unsigned int size;
        const std::vector<transaction>& previous;

        // Keep track of which txs have been matched.
        std::vector<bool> matched;

        // Keep track of which txs have been seen;
        std::vector<bool> seen;

        std::vector<libbitcoin::chain::transaction> txs;
        std::vector<libbitcoin::hash_digest> hashes;

        libbitcoin::chain::output get(
            libbitcoin::chain::output_point previous_output)
        {
            for (unsigned int i = 0; i < size; i++) {
                // skip txs that have already been matched.
                if (matched[i]) continue;

                // The previous tx corresponding to this index.
                transaction prev;

                // The hash of the previous tx corresponding to this index.
                libbitcoin::hash_digest id;

                if (seen[i]) {
                    prev = txs[i];
                    id = hashes[i];
                } else {
                    prev = previous[i];
                    id = prev.hash();

                    // libbitcoin::reverse_hash(id);
                }

                // If the hashes don't match, save these and try the next one.
                if (id != previous_output.hash()) {
                    txs[i] = prev;
                    hashes[i] = id;
                    seen[i] = true;

                    continue;
                }

                matched[i] = true;

                auto outputs = prev.outputs();
                if (previous_output.index() < outputs.size()) {
                    return prev.outputs()[previous_output.index()];
                }

                // Error case if the input doesn't correspond to an output.
                return libbitcoin::chain::output();
            }

            // Invalid output if we go through the whole list and don't find
            // what we're looking for.
            return libbitcoin::chain::output();
        }

        matcher(int z, const std::vector<transaction>& p)
            : size(z)
            , previous(p)
            , matched(std::vector<bool>(size))
            , seen(std::vector<bool>(size))
            , txs(std::vector<libbitcoin::chain::transaction>(size))
            , hashes(std::vector<libbitcoin::hash_digest>(size))
        {
            for (int i = 0; i < z; i++) {
                matched[i] = false;
                seen[i] = false;
            }
        }
    };

    matcher m(size, previous);

    // Go through inputs of this transaction.
    for (std::vector<libbitcoin::chain::input>::iterator input = inputs.begin();
         input != inputs.end();
         ++input) {

        // Get the previous output corresponding to this input.
        libbitcoin::chain::output output = m.get(input->previous_output());

        // if the output is invalid, something went wrong.
        if (!output.is_valid()) {
            return false;
        }

        if (extract_designated_pubkey(
                out,
                input->script().operations(),
                output.script().operations())) {
            return true;
        }
    }

    return false;
}

}
