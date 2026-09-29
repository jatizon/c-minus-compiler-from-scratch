extern "C" {
#include "src/helpers/hash.h"
}
#include <gtest/gtest.h>


TEST(HashTest, Djb2HashMatchesKnownValues) {
    // Second argument is num_buckets: djb2_hash takes `% num_buckets` after
    // every character, so it always returns a valid bucket index directly.
    EXPECT_EQ(djb2_hash((void*) "", 0), 5381u);
    EXPECT_EQ(djb2_hash((void*) "a", 1), 0u);
    EXPECT_EQ(djb2_hash((void*) "hello", 5), 1u);
    EXPECT_EQ(djb2_hash((void*) "The quick brown fox", 19), 18u);
}
