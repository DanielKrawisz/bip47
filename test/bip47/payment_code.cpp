#include <bip47/secret.hpp>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "test_keys.hpp"

using namespace std;

namespace bip47
{
    
TEST(hd, equal) {
    for (int i = 0; i < test_keys.size(); i++) 
        for (int j = 0; j < test_keys.size(); j++) 
            if (i == j) {
                ASSERT_NO_THROW(EXPECT_TRUE(test_keys[i] == test_keys[j]));
                ASSERT_NO_THROW(EXPECT_FALSE(test_keys[i] != test_keys[j]));
            } else {
                ASSERT_NO_THROW(EXPECT_TRUE(test_keys[i] != test_keys[j]));
                ASSERT_NO_THROW(EXPECT_FALSE(test_keys[i] == test_keys[j]));
            }
}

TEST(hd, data) {
    for (hd_pair hd : test_keys) {
        ASSERT_NO_THROW(EXPECT_TRUE(hd.secret == hd_secret::from_data(hd.secret.data())));
        ASSERT_NO_THROW(EXPECT_TRUE(hd.pubkey == hd_public::from_data(hd.pubkey.data())));
    }
}

TEST(payment_codes, base58_success) {
    for (test_payment_code test_case : valid_test_payment_codes) {
        payment_code code = payment_code::base58_decode(test_case.code);
        ASSERT_NO_THROW(EXPECT_TRUE(code.valid()));
        ASSERT_NO_THROW(EXPECT_TRUE(test_case.code == code.base58()));
    }
}
    
TEST(payment_codes, version) {
    payment_code_version expected = 0;
    for (test_payment_code test_case : valid_test_payment_codes) {
        expected = next_test_version(expected);
        ASSERT_NO_THROW(EXPECT_EQ(expected, payment_code::base58_decode(test_case.code).version())); 
    }
}

TEST(payment_codes, base58_fail) {
    for (test_payment_code test_case: invalid_test_payment_codes) {
        ASSERT_NO_THROW(EXPECT_FALSE(payment_code::base58_decode(test_case.code).valid()));
    }
}

TEST(payment_codes, pubkey) {
    for (test_payment_code test_case : valid_test_payment_codes) {
        payment_code code = payment_code::base58_decode(test_case.code);
        ASSERT_NO_THROW(EXPECT_TRUE(test_case.key.pubkey() == code.pubkey()));
    }
}

} // bip47
