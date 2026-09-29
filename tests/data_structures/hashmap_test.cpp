extern "C" {
#include "src/data_structures/hashmap.h"
#include "src/helpers/hash.h"
}
#include <cstdio>
#include <gtest/gtest.h>


static bool is_prime(int n) {
    if (n < 2)
        return false;

    for (int i = 2; i * i <= n; ++i)
        if (n % i == 0)
            return false;

    return true;
}

static size_t always_bucket_zero(void* key, size_t num_buckets) {
    (void) key;
    (void) num_buckets;
    return 0;
}


TEST(HashmapTest, CreatesHashmap) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    EXPECT_EQ(hashmap.size, 0u);
}

TEST(HashmapTest, EmptyHashmapStartsWithSizeZero) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));

    EXPECT_EQ(hashmap.size, 0u);
    EXPECT_EQ(hashmap.capacity, 0u);
}

TEST(HashmapTest, GetOnEmptyHashmapReturnsNull) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));

    EXPECT_EQ(hashmap_get(&hashmap, (char*) "missing"), nullptr);
}

TEST(HashmapTest, ContainsOnEmptyHashmapReturnsFalse) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));

    EXPECT_FALSE(hashmap_contains(&hashmap, (char*) "missing"));
}

TEST(HashmapTest, CapacityDoublesOnGrowth) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int a = 1, b = 2, c = 3;

    hashmap_set(&hashmap, (char*) "a", &a);
    EXPECT_EQ(hashmap.capacity, 1u);

    hashmap_set(&hashmap, (char*) "b", &b);
    EXPECT_EQ(hashmap.capacity, 2u);

    hashmap_set(&hashmap, (char*) "c", &c);
    EXPECT_EQ(hashmap.capacity, 4u);
}

TEST(HashmapTest, HappyPathSetAndGet) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int a = 10, b = 20, c = 30;

    hashmap_set(&hashmap, (char*) "a", &a);
    hashmap_set(&hashmap, (char*) "b", &b);
    hashmap_set(&hashmap, (char*) "c", &c);

    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "a"), 10);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "b"), 20);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "c"), 30);
}

TEST(HashmapTest, ContainsReturnsTrueAfterSet) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int value = 42;

    hashmap_set(&hashmap, (char*) "key", &value);

    EXPECT_TRUE(hashmap_contains(&hashmap, (char*) "key"));
}

TEST(HashmapTest, GetReturnsNullWhenKeyNotFound) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int value = 42;

    hashmap_set(&hashmap, (char*) "key", &value);

    EXPECT_EQ(hashmap_get(&hashmap, (char*) "missing"), nullptr);
}

TEST(HashmapTest, SetOverwritesExistingKey) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int first = 1, second = 2;

    hashmap_set(&hashmap, (char*) "key", &first);
    hashmap_set(&hashmap, (char*) "key", &second);

    EXPECT_EQ(hashmap.size, 1u);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "key"), 2);
}

TEST(HashmapTest, SetOnCollidingBucketsResolvesCorrectly) {
    Hashmap hashmap = hashmap_new(always_bucket_zero, 0.75f, sizeof(int));
    int value_a = 1, value_b = 2;

    hashmap_set(&hashmap, (char*) "a", &value_a);
    hashmap_set(&hashmap, (char*) "b", &value_b);

    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "a"), 1);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "b"), 2);
}

TEST(HashmapTest, EntriesSurviveResize) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int a = 1, b = 2;

    hashmap_set(&hashmap, (char*) "a", &a);
    ASSERT_EQ(hashmap.capacity, 1u);

    hashmap_set(&hashmap, (char*) "b", &b);
    ASSERT_GT(hashmap.capacity, 1u);

    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "a"), 1);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "b"), 2);
}

TEST(HashmapTest, NumBucketsForCapacityRespectsLoadFactor) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    size_t num_buckets = hashmap_get_num_buckets_for_capacity(&hashmap, 100);

    EXPECT_TRUE(is_prime((int) num_buckets));
    EXPECT_GE(num_buckets, 100u);
}

TEST(HashmapTest, DifferentLoadFactorThresholdsProduceDifferentBucketCounts) {
    Hashmap low_threshold_hashmap = hashmap_new(djb2_hash, 0.5f, sizeof(int));
    Hashmap high_threshold_hashmap = hashmap_new(djb2_hash, 0.9f, sizeof(int));

    size_t num_buckets_low = hashmap_get_num_buckets_for_capacity(&low_threshold_hashmap, 100);
    size_t num_buckets_high = hashmap_get_num_buckets_for_capacity(&high_threshold_hashmap, 100);

    EXPECT_NE(num_buckets_low, num_buckets_high);
}

TEST(HashmapTest, SetSameKeyManyTimesKeepsSizeConstant) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int values[5] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; ++i)
        hashmap_set(&hashmap, (char*) "key", &values[i]);

    EXPECT_EQ(hashmap.size, 1u);
    EXPECT_EQ(*(int*) hashmap_get(&hashmap, (char*) "key"), 5);
}

TEST(HashmapTest, HandlesManyKeysAcrossMultipleResizes) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));

    const int NUM_KEYS = 60;
    static char keys[NUM_KEYS][16];
    static int values[NUM_KEYS];

    for (int i = 0; i < NUM_KEYS; ++i) {
        snprintf(keys[i], sizeof(keys[i]), "key%d", i);
        values[i] = i;
        hashmap_set(&hashmap, keys[i], &values[i]);
    }

    EXPECT_EQ(hashmap.size, (size_t) NUM_KEYS);

    for (int i = 0; i < NUM_KEYS; ++i)
        EXPECT_EQ(*(int*) hashmap_get(&hashmap, keys[i]), i);
}

TEST(HashmapTest, RemoveDeletesKeyAndDecreasesSize) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int value = 42;

    hashmap_set(&hashmap, (char*) "key", &value);
    ASSERT_EQ(hashmap.size, 1u);

    hashmap_remove(&hashmap, (char*) "key");

    EXPECT_EQ(hashmap.size, 0u);
    EXPECT_EQ(hashmap_get(&hashmap, (char*) "key"), nullptr);
}

TEST(HashmapTest, RemoveOnMissingKeyIsNoOp) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int value = 42;

    hashmap_set(&hashmap, (char*) "key", &value);
    ASSERT_EQ(hashmap.size, 1u);

    hashmap_remove(&hashmap, (char*) "missing");

    EXPECT_EQ(hashmap.size, 1u);
}

TEST(HashmapTest, ContainsReturnsFalseAfterRemove) {
    Hashmap hashmap = hashmap_new(djb2_hash, 0.75f, sizeof(int));
    int value = 42;

    hashmap_set(&hashmap, (char*) "key", &value);
    ASSERT_TRUE(hashmap_contains(&hashmap, (char*) "key"));

    hashmap_remove(&hashmap, (char*) "key");

    EXPECT_FALSE(hashmap_contains(&hashmap, (char*) "key"));
}
