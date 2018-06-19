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
        ASSERT_NO_THROW(EXPECT_TRUE(hd.secret == hd_secret::from_data(hd.secret.data())));
        ASSERT_NO_THROW(EXPECT_TRUE(hd.pubkey == hd_public::from_data(hd.pubkey.data())));
    }
}

// Ensure that the chain code is the same for public and private versions of the hd key. 
TEST(hd, chain_code) {
    for (hd_pair hd : test_keys) {
        ASSERT_NO_THROW(EXPECT_TRUE(hd.secret.chain_code == hd.pubkey.chain_code));
        
        ASSERT_NO_THROW(EXPECT_TRUE(low::chain_code(hd.libbitcoin_hd_key_pubkey()) == hd.pubkey.chain_code));
        ASSERT_NO_THROW(EXPECT_TRUE(low::chain_code(hd.libbitcoin_hd_key_secret()) == hd.secret.chain_code));
    }
}

// The next tests ensure that my hd_secret and hd_pubkey types work the same as
// libbitcoin's versions of them. 
TEST(hd, libbitcoin_hd_key_pubkey) {
    for (hd_pair hd : test_keys) {
        auto hdp = hd.libbitcoin_hd_key_pubkey();
        ASSERT_NO_THROW(EXPECT_TRUE(hd_public(hdp) == hd.pubkey));
    }
}

TEST(hd, libbitcoin_hd_key_secret) {
    for (hd_pair hd : test_keys) {
        auto hds = hd.libbitcoin_hd_key_secret();
                
        ASSERT_NO_THROW(EXPECT_TRUE(hd_secret(hds).key == hd.secret.key));
                
        ASSERT_NO_THROW(EXPECT_TRUE(hd_secret(hds).chain_code == hd.secret.chain_code));
                
        ASSERT_NO_THROW(EXPECT_TRUE(hd_secret(hds) == hd.secret));
        
        ASSERT_NO_THROW(EXPECT_TRUE(hd_public(low::to_public(hds)) == hd_secret(hds).pubkey()));
    }
}

// This test ensures that the hd_secret and hd_public types behave the same way as the
// libbitcoin versions. 
TEST(hd, pubkey) {
    for (hd_pair hd : test_keys) {
        auto hds = hd.libbitcoin_hd_private();
        auto hdp = hd.libbitcoin_hd_public();
        
        ASSERT_NO_THROW(EXPECT_TRUE(hd_public(hdp.to_hd_key()) == hd.pubkey));
                
        ASSERT_NO_THROW(EXPECT_TRUE(hd_secret(hds.to_hd_key()) == hd.secret));
        
        ASSERT_NO_THROW(EXPECT_TRUE(hd_public(hds.to_public().to_hd_key()) == hd_secret(hds.to_hd_key()).pubkey()));
    }
}

} // bip47
