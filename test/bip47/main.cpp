#include <gtest/gtest.h>
#include "hd.cpp"
#include "payment_code.cpp"

int main(int arc, char* argv[]) {
    testing::InitGoogleTest(&arc, argv);
    return RUN_ALL_TESTS();
}
