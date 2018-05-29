#include <gtest/gtest.h>
#include <bip47/secret.hpp>
#include <string>
#include <vector>

using namespace std;

namespace bip47
{

// TODO fill in some test cases
TEST(payment_codes, base58_success) {
    const vector<string> test_cases = {};
    for (string test_case : test_cases) {
        payment_code code = payment_code::base58_decode(test_case);
        EXPECT_TRUE(code.valid());
        EXPECT_TRUE(test_case == code.base58_encode());
    }
}

TEST(payment_codes, base58_fail) {
    const vector<string> test_cases = {};
    for (string test_case: test_cases) {
        EXPECT_FALSE(payment_code::base58_decode(test_case).valid());
    }
}

} // bip47
