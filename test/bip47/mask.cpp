#include <gtest/gtest.h>
#include "test_keys.hpp"
#include <bip47/secret.hpp>
#include <bip47/notification.hpp>

namespace bip47
{
    
struct mask_test_case {
    secret from;
    secret to;
    keypair designated;
};

const std::vector<mask_test_case> generate_mask_test_cases(std::vector<test_payment_code> x) {
    std::vector<mask_test_case> test_cases(x.size());
    
    for (int i = 0; i < x.size(); i++) {
        test_cases[i].from = x[i].make_secret();
        test_cases[(i + 1)%x.size()].to = test_cases[i].from;
        test_cases[(i + 2)%x.size()].designated = x[i].make_keypair();
    }
    
    return test_cases;
}

const std::vector<mask_test_case> mask_test_cases = generate_mask_test_cases(valid_test_payment_codes);

TEST(payment_code, masks) {
    for (mask_test_case test_case : mask_test_cases) {
        payment_code masked;
        libbitcoin::chain::output_point output(libbitcoin::null_hash, 0);
        ASSERT_NO_THROW(masked = low::mask_payment_code(test_case.from.code, 
            test_case.designated.key, test_case.to.code.point(), output);
        ASSERT_TRUE(low::unmask_payment_code(masked, test_case.designated.point, test_case.to.key.key, output)));
        ASSERT_NO_THROW(EXPECT_TRUE(masked == test_case.from.code));
    }
}

} // bip47
