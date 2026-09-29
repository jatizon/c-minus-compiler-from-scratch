extern "C" {
#include "src/data_structures/vector.h"
}
#include <gtest/gtest.h>


static bool int_equals(void* a, void* b) {
    return *(int*) a == *(int*) b;
}


TEST(VectorTest, CreatesVector) {
    Vector vector = vector_new(sizeof(int));
    EXPECT_EQ(vector.size, 0u);
}

TEST(VectorTest, PopOnEmptyVectorIsIdempotentNoOp) {
    Vector vector = vector_new(sizeof(int));

    EXPECT_EQ(vector_pop(&vector), nullptr);
    EXPECT_EQ(vector.size, 0u);
}

TEST(VectorTest, GetOnEmptyVectorDies) {
    Vector vector = vector_new(sizeof(int));

    EXPECT_DEATH(vector_get_element_ptr(&vector, 0), ".*");
}

TEST(VectorTest, GetAtSizeIndexDies) {
    Vector vector = vector_new(sizeof(int));
    int value = 42;
    vector_push(&vector, &value);

    EXPECT_DEATH(vector_get_element_ptr(&vector, vector.size), ".*");
}

TEST(VectorTest, EmptyVectorStartsWithSizeZero) {
    Vector vector = vector_new(sizeof(int));

    EXPECT_EQ(vector.size, 0u);
    EXPECT_EQ(vector.capacity, 0u);
}

TEST(VectorTest, CapacityDoublesOnGrowth) {
    Vector vector = vector_new(sizeof(int));
    int value = 0;

    vector_push(&vector, &value);
    EXPECT_EQ(vector.capacity, 1u);

    vector_push(&vector, &value);
    EXPECT_EQ(vector.capacity, 2u);

    vector_push(&vector, &value);
    EXPECT_EQ(vector.capacity, 4u);
}

TEST(VectorTest, PopDecreasesSize) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20;

    vector_push(&vector, &a);
    vector_push(&vector, &b);
    ASSERT_EQ(vector.size, 2u);

    vector_pop(&vector);
    EXPECT_EQ(vector.size, 1u);

    vector_pop(&vector);
    EXPECT_EQ(vector.size, 0u);
}

TEST(VectorTest, PopReturnsLastElement) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20;

    vector_push(&vector, &a);
    vector_push(&vector, &b);

    void* popped = vector_pop(&vector);
    EXPECT_EQ(*(int*) popped, 20);
}

TEST(VectorTest, HappyPathPushAndGet) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20, c = 30;

    vector_push(&vector, &a);
    vector_push(&vector, &b);
    vector_push(&vector, &c);

    ASSERT_EQ(vector.size, 3u);

    EXPECT_EQ(*(int*) vector_get_element_ptr(&vector, 0), 10);
    EXPECT_EQ(*(int*) vector_get_element_ptr(&vector, 1), 20);
    EXPECT_EQ(*(int*) vector_get_element_ptr(&vector, 2), 30);
}

TEST(VectorTest, FindOnEmptyVectorReturnsNegativeOne) {
    Vector vector = vector_new(sizeof(int));
    int target = 10;

    EXPECT_EQ(vector_find(&vector, &target, int_equals), -1);
}

TEST(VectorTest, FindReturnsIndexOfMatchingElement) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20, c = 30;
    vector_push(&vector, &a);
    vector_push(&vector, &b);
    vector_push(&vector, &c);

    int target = 20;
    int result = vector_find(&vector, &target, int_equals);

    EXPECT_EQ(result, 1);
}

TEST(VectorTest, FindReturnsNegativeOneWhenNotFound) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20;
    vector_push(&vector, &a);
    vector_push(&vector, &b);

    int target = 99;
    EXPECT_EQ(vector_find(&vector, &target, int_equals), -1);
}

TEST(VectorTest, FindReturnsFirstMatchOnDuplicates) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20, c = 10;
    vector_push(&vector, &a);
    vector_push(&vector, &b);
    vector_push(&vector, &c);

    int target = 10;
    int result = vector_find(&vector, &target, int_equals);

    EXPECT_EQ(result, 0);
}

TEST(VectorTest, RemoveShiftsSubsequentElementsAndDecreasesSize) {
    Vector vector = vector_new(sizeof(int));
    int a = 10, b = 20, c = 30;
    vector_push(&vector, &a);
    vector_push(&vector, &b);
    vector_push(&vector, &c);

    vector_remove(&vector, 0);

    ASSERT_EQ(vector.size, 2u);
    EXPECT_EQ(*(int*) vector_get_element_ptr(&vector, 0), 20);
    EXPECT_EQ(*(int*) vector_get_element_ptr(&vector, 1), 30);
}

TEST(VectorTest, RemoveAtInvalidIndexDies) {
    Vector vector = vector_new(sizeof(int));
    int value = 10;
    vector_push(&vector, &value);

    EXPECT_DEATH(vector_remove(&vector, 1), ".*");
}
