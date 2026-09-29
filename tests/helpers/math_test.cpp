extern "C" {
#include "src/helpers/math.h"
}
#include <gtest/gtest.h>


TEST(MathTest, NextPrimeAfterPrime) {
    EXPECT_EQ(next_prime(2), 3);
    EXPECT_EQ(next_prime(3), 5);
    EXPECT_EQ(next_prime(7), 11);
    EXPECT_EQ(next_prime(13), 17);
}

TEST(MathTest, NextPrimeAfterComposite) {
    EXPECT_EQ(next_prime(4), 5);
    EXPECT_EQ(next_prime(8), 11);
    EXPECT_EQ(next_prime(9), 11);
    EXPECT_EQ(next_prime(10), 11);
    EXPECT_EQ(next_prime(14), 17);
    EXPECT_EQ(next_prime(20), 23);
}

TEST(MathTest, NextPrimeAcrossLargeGap) {
    EXPECT_EQ(next_prime(89), 97);
}

TEST(MathTest, NextPrimeForLargerValue) {
    EXPECT_EQ(next_prime(97), 101);
    EXPECT_EQ(next_prime(100), 101);
    EXPECT_EQ(next_prime(1000), 1009);
}

TEST(MathTest, NextPrimeIsCorrectBetweenTenThousandAndHundredThousand) {
    EXPECT_EQ(next_prime(10007), 10009);
    EXPECT_EQ(next_prime(12345), 12347);
    EXPECT_EQ(next_prime(19999), 20011);
    EXPECT_EQ(next_prime(25000), 25013);
    EXPECT_EQ(next_prime(33333), 33343);
    EXPECT_EQ(next_prime(41957), 41959);
    EXPECT_EQ(next_prime(50000), 50021);
    EXPECT_EQ(next_prime(63997), 64007);
    EXPECT_EQ(next_prime(71234), 71237);
    EXPECT_EQ(next_prime(80001), 80021);
    EXPECT_EQ(next_prime(89999), 90001);
    EXPECT_EQ(next_prime(91234), 91237);
    EXPECT_EQ(next_prime(99991), 100003);
    EXPECT_EQ(next_prime(100000), 100003);
}
