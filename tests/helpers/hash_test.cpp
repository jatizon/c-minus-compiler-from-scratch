extern "C" {
#include "../../src/helpers/hash.h"
}
#include <gtest/gtest.h>


TEST(HashTest, Djb2HashMatchesKnownValues) {
    EXPECT_EQ(djb2_hash((void*) "", 0), 5381u);
    EXPECT_EQ(djb2_hash((void*) "a", 1), 177670u);
    EXPECT_EQ(djb2_hash((void*) "hello", 5), 210714636441u);
    EXPECT_EQ(djb2_hash((void*) "The quick brown fox", 19), 9925230662628146424u);
}
