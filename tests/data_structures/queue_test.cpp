extern "C" {
#include "src/data_structures/queue.h"
}
#include <gtest/gtest.h>


TEST(QueueTest, CreatesQueue) {
    Queue queue = queue_new(sizeof(int));
    EXPECT_EQ(queue.size, 0u);
}

TEST(QueueTest, EmptyQueueStartsEmpty) {
    Queue queue = queue_new(sizeof(int));
    EXPECT_TRUE(queue_empty(&queue));
}

TEST(QueueTest, PushIncreasesSize) {
    Queue queue = queue_new(sizeof(int));
    int value = 42;

    queue_push(&queue, &value);

    EXPECT_EQ(queue.size, 1u);
    EXPECT_FALSE(queue_empty(&queue));
}

TEST(QueueTest, FrontReturnsFirstPushedElement) {
    Queue queue = queue_new(sizeof(int));
    int a = 10, b = 20;

    queue_push(&queue, &a);
    queue_push(&queue, &b);

    EXPECT_EQ(*(int*) queue_front(&queue), 10);
}

TEST(QueueTest, PopRemovesFrontAndDecreasesSize) {
    Queue queue = queue_new(sizeof(int));
    int a = 10, b = 20;

    queue_push(&queue, &a);
    queue_push(&queue, &b);
    ASSERT_EQ(queue.size, 2u);

    void* popped = queue_pop(&queue);

    EXPECT_EQ(*(int*) popped, 10);
    EXPECT_EQ(queue.size, 1u);
    EXPECT_EQ(*(int*) queue_front(&queue), 20);
}

TEST(QueueTest, FifoOrderIsPreservedAcrossGrowth) {
    Queue queue = queue_new(sizeof(int));

    const int NUM_VALUES = 10;
    int values[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; ++i) {
        values[i] = i;
        queue_push(&queue, &values[i]);
    }

    ASSERT_EQ(queue.size, (size_t) NUM_VALUES);

    for (int i = 0; i < NUM_VALUES; ++i) {
        EXPECT_EQ(*(int*) queue_front(&queue), i);
        queue_pop(&queue);
    }

    EXPECT_TRUE(queue_empty(&queue));
}

TEST(QueueTest, WraparoundAfterPopAndPushKeepsCorrectOrder) {
    Queue queue = queue_new(sizeof(int));
    int a = 1, b = 2, c = 3, d = 4;

    queue_push(&queue, &a);
    queue_push(&queue, &b);
    queue_pop(&queue);
    queue_push(&queue, &c);
    queue_push(&queue, &d);

    EXPECT_EQ(*(int*) queue_front(&queue), 2);
    queue_pop(&queue);

    EXPECT_EQ(*(int*) queue_front(&queue), 3);
    queue_pop(&queue);

    EXPECT_EQ(*(int*) queue_front(&queue), 4);
    queue_pop(&queue);

    EXPECT_TRUE(queue_empty(&queue));
}

TEST(QueueTest, QueueFullIsTrueWhenSizeEqualsCapacity) {
    Queue queue = queue_new(sizeof(int));
    int value = 1;

    queue_push(&queue, &value);

    EXPECT_TRUE(queue_full(&queue));
}

TEST(QueueTest, QueueFullIsFalseRightAfterGrowth) {
    Queue queue = queue_new(sizeof(int));
    int a = 1, b = 2, c = 3;

    queue_push(&queue, &a);
    queue_push(&queue, &b);
    queue_push(&queue, &c);

    EXPECT_FALSE(queue_full(&queue));
}
