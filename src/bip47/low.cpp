#include <bip47/low.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include "designated.cpp"

namespace bip47
{

namespace low
{

const output inline pay_key_hash(const payment_code& to, unsigned int amount, address_format format) {
    return output(amount, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_key_hash_pattern(to.notification_address(format))));
}

const bool inline to(const output& output, const address& notification_address) {
    const auto ops = output.script().operations();
    return (libbitcoin::chain::script::is_pay_key_hash_pattern(ops) && address(ops[2].data()) == notification_address);
}

const bool payload(payment_code& out, const output& output) {
    const auto ops = output.script().operations();
    if (!libbitcoin::chain::script::is_pay_null_data_pattern(ops)) return false;
    if (ops[1].data().size() != 80) return false;
    out = payment_code(ops[1].data());
    return true;
}

bool inline identifier_equals(const ec_compressed &x, const data_chunk& y) {
    if (y.size() != libbitcoin::ec_compressed_size) return false;
    for (int i = 0; i < libbitcoin::ec_compressed_size; i++) if (x[i] != y[i]) return false;
    return true;
}

namespace v1
{

const inline output notification_output(
    const payment_code& alice,
    const payment_code& bob,
    const outpoint& prior, 
    const ec_private& designated) {
    return output(0, libbitcoin::chain::script(libbitcoin::chain::script::to_pay_null_data_pattern(alice.mask(designated, bob.point(), prior))));
}

bool inline is_notification_output(const output& output) {
    const auto ops = output.script().operations();
    return libbitcoin::chain::script::is_pay_null_data_pattern(ops) && ops[1].data().size() == 80;
}

bool inline notification_payload(payment_code& payload, const transaction& tx) {
    for (auto output : tx.outputs()) if (low::payload(payload, output)) return true;
    return false;
}

} // v1

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

namespace v2
{

bool inline is_notification_change_output(const output& output) {
    return is_notification_change_output_pattern(output.script().operations());
}

const output inline notification_change_output(const ec_compressed& alice, const payment_code& bob, unsigned int amount) {
    // TODO randomization/ordering policy? 
    return output(amount, libbitcoin::chain::script::to_pay_multisig_pattern(1, {alice, bob.identifier()}));
}

const inline bool is_notification_change_output_to(const output& output, const identifier& bob_id) {
    if (!is_notification_change_output(output)) return false;
    identifier_equals(bob_id, output.script().operations()[1].data());
    return true;
}

bool inline notification_payload(payment_code& payload, const transaction& tx) {
    return v1::notification_payload(payload, tx);
}

} // v2

// The previous transactions are not necessarily given in order.
bool designated_pubkey(
    ec_public& out,
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

} // low

} // bip47
