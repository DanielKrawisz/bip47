#include <bip47/patterns.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin
{

namespace chain
{

bool is_public_key_pattern(const machine::operation::list& ops)
{
    return ops.size() == 3 && is_public_key(ops[1].data()) &&
           ops[2].code() == machine::opcode::checksig;
}

bool is_key_hash_pattern(const machine::operation::list& ops)
{
    return ops.size() == 7 && is_public_key(ops[1].data()) &&
           ops[2].code() == machine::opcode::dup && ops[3].code() == machine::opcode::hash160 &&
           ops[4].data().size() == short_hash_size &&
           ops[5].code() == machine::opcode::equalverify &&
           ops[6].code() == machine::opcode::checksig;
}

bool is_multisig_script_hash_pattern(
    unsigned int& middle,
    const machine::operation::list& ops)
{
    static constexpr auto op_1 = static_cast<uint8_t>(machine::opcode::push_positive_1);
    static constexpr auto op_16 =
        static_cast<uint8_t>(machine::opcode::push_positive_16);

    const auto op_count = ops.size();

    if (op_count < 6 || ops[0].code() != machine::opcode::push_size_0 ||
        ops[op_count - 1].code() != machine::opcode::checkmultisig)
        return false;

    const auto op_n = static_cast<uint8_t>(ops[op_count - 2].code());

    int num_signatures = 0;
    middle = 1;
    while (true) {
        if (middle >= op_count) {
            return false;
        }

        // The first numeric op we encounter is the middle of the script.
        if (machine::operation::is_numeric(ops[middle].code())) {
            break;
        }

        // check for push data
        if (!machine::operation::is_push(ops[middle].code())) {
            return false;
        }

        num_signatures++;
        middle++;
    }

    const auto op_m = static_cast<uint8_t>(ops[middle].code());

    if (op_m < op_1 || op_m > op_n || op_n < op_1 || op_n > op_16) return false;

    const int number = op_n - op_1 + 1u;
    const int points = op_count - 3u - num_signatures - 1;

    if (number != points) return false;

    for (auto op = ops.begin() + num_signatures + 3; op != ops.end() - 2; ++op)
        if (!is_public_key(op->data())) return false;

    return true;
}

}  // chain

}  // libbitcoin
