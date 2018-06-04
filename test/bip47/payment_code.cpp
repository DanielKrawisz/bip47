#include <gtest/gtest.h>
#include <bip47/secret.hpp>
#include <string>
#include <vector>
#include "test_keys.cpp"

using namespace std;

namespace bip47
{

TEST(payment_codes, base58_success) {
    for (test_payment_code test_case : valid_test_payment_codes) {
        payment_code code = payment_code::base58_decode(test_case.code);
        EXPECT_TRUE(code.valid());
        EXPECT_TRUE(test_case.code == code.base58_encode());
    }
}

TEST(payment_codes, base58_fail) {
    for (test_payment_code test_case: invalid_test_payment_codes) {
        EXPECT_FALSE(payment_code::base58_decode(test_case.code).valid());
    }
}

TEST(payment_codes, pubkey) {
    for (test_payment_code test_case : valid_test_payment_codes) {
        payment_code code = payment_code::base58_decode(test_case.code);
        EXPECT_TRUE(test_case.key.pubkey() == code.pubkey());
    }
}

TEST(payment_codes, masks) {
}

TEST(payment_codes, notification_address) {
}

} // bip47
