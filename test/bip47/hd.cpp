#include <bip47/secret.hpp>
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
        hd_secret hds;
        ASSERT_TRUE(hd::from_data(hd::data(hd.Secret), hds));
        ASSERT_NO_THROW(EXPECT_TRUE(hd.Secret == hds));
        hd_public hdp;
        ASSERT_TRUE(hd::from_data(hd::data(hd.Pubkey), hdp));
        ASSERT_NO_THROW(EXPECT_TRUE(hd.Pubkey == hdp));
    }
}

// Ensure that the chain code is the same for public and private versions of the hd key. 
TEST(hd, chain_code) {
    for (hd_pair hd : test_keys) {
        ASSERT_NO_THROW(EXPECT_TRUE(hd.Secret.Pubkey.ChainCode == hd.Pubkey.ChainCode));
        
        ASSERT_NO_THROW(EXPECT_TRUE(low::chain_code(hd.libbitcoin_hd_key_pubkey()) == hd.Pubkey.ChainCode));
        ASSERT_NO_THROW(EXPECT_TRUE(low::chain_code(hd.libbitcoin_hd_key_secret()) == hd.Secret.Pubkey.ChainCode));
    }
}

TEST(hd, libbitcoin_hd_key_secret) {
    for (hd_pair hd : test_keys) {
        auto hds = hd.libbitcoin_hd_key_secret();
                
        ASSERT_NO_THROW(EXPECT_TRUE(low::secret_key(hds) == hd.Secret.Secret));
                
        ASSERT_NO_THROW(EXPECT_TRUE(low::chain_code(hds) == hd.Secret.Pubkey.ChainCode));
    }
}

} // bip47
