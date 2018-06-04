#ifndef BIP47_PATTERNS_HPP
#define BIP47_PATTERNS_HPP

#include <bitcoin/bitcoin/machine/operation.hpp>
#include <vector>

// Some patterns that we need do not exist in libbitcoin on their own. 
namespace libbitcoin
{

namespace chain
{

bool is_public_key_pattern(const machine::operation::list& ops);
bool is_key_hash_pattern(const machine::operation::list& ops);
bool is_multisig_script_hash_pattern(unsigned int& middle, const machine::operation::list& ops);

machine::operation::list to_pay_multisig_pattern(uint8_t signatures, const std::vector<data_chunk>& points);

} // chain
    
} // libbitcoin

#endif
