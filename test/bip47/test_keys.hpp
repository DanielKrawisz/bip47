#ifndef BIP47_TEST_KEYS_HPP
#define BIP47_TEST_KEYS_HPP

#include <bip47/secret.hpp>
#include <string>
#include <vector>

using namespace bip47;

struct hd_pair {
    hd_secret Secret;
    hd_public Pubkey;
    
    bool operator==(const hd_pair& p) const {
        return Secret == p.Secret && Pubkey == p.Pubkey;
    }
    
    bool operator!=(const hd_pair& p) const {
        return Secret != p.Secret || Pubkey != p.Pubkey;
    }
    
    const libbitcoin::wallet::hd_key libbitcoin_hd_key_pubkey();
    const libbitcoin::wallet::hd_key libbitcoin_hd_key_secret();
    
    const libbitcoin::wallet::hd_private libbitcoin_hd_private();
    const libbitcoin::wallet::hd_public libbitcoin_hd_public();
};
    
struct keypair {
    ec_secret key;
    ec_compressed point;
};

struct test_payment_code {
    hd_secret key;
    std::string code;
    
    test_payment_code(payment_code_version version, const hd_secret pk, bool bitmessage_notification);
    
    test_payment_code():key(),code(""){}
    
    payment_code make_payment_code() {
        return payment_code::base58_decode(code);
    }
    
    secret make_secret() {
        auto code = make_payment_code();
        return bip47::secret(code.version(), code.bitmessage_notification(), key);
    }
    
    keypair make_keypair() {
        auto code = make_payment_code();
        return {key.Secret, code.point()};
    }
};

payment_code_version next_test_version(payment_code_version test_version);

// A  bunch of randomly generated test hd keys in base 16.
const extern std::vector<hd_pair> test_keys;

// Generate some version 2 payment codes from the hd keys given earlier. 
const extern std::vector<test_payment_code> valid_test_payment_codes;
const extern std::vector<test_payment_code> invalid_test_payment_codes;

#endif

