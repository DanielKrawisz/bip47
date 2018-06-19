#ifndef BIP47_TEST_KEYS_HPP
#define BIP47_TEST_KEYS_HPP

#include <bip47/payment_code.hpp>
#include <string>
#include <vector>

using namespace bip47;

struct hd_pair {
    hd_secret secret;
    hd_public pubkey;
    
    bool operator==(hd_pair p) const {
        return secret == p.secret && pubkey == p.pubkey;
    }
    
    bool operator!=(hd_pair p) const {
        return secret != p.secret || pubkey != p.pubkey;
    }
    
    const libbitcoin::wallet::hd_key libbitcoin_hd_key_pubkey();
    const libbitcoin::wallet::hd_key libbitcoin_hd_key_secret();
    
    const libbitcoin::wallet::hd_private libbitcoin_hd_private();
    const libbitcoin::wallet::hd_public libbitcoin_hd_public();
};

struct test_payment_code {
    hd_secret key;
    std::string code;
    
    test_payment_code(payment_code_version version, const hd_secret pk, bool bitmessage_notification);
    
    test_payment_code():key(),code(""){}
};

payment_code_version next_test_version(payment_code_version test_version);

// A  bunch of randomly generated test hd keys in base 16.
const extern std::vector<hd_pair> test_keys;

// Generate some version 2 payment codes from the hd keys given earlier. 
const extern std::vector<test_payment_code> valid_test_payment_codes;
const extern std::vector<test_payment_code> invalid_test_payment_codes;

#endif

