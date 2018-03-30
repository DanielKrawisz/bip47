#ifndef BIP47_PAYMENT_CODE_HPP
#define BIP47_PAYMENT_CODE_HPP

#include <bitcoin/bitcoin/machine/operation.hpp>

namespace libbitcoin
{

namespace chain
{

bool is_public_key_pattern(const machine::operation::list& ops);
bool is_key_hash_pattern(const machine::operation::list& ops);
bool is_multisig_script_hash_pattern(unsigned int& middle, const machine::operation::list& ops);

} // chain
    
} // libbitcoin

#endif
