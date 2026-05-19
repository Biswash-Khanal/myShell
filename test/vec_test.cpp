#include <gtest/gtest.h>
extern "C" {
#include "Vec.h"
#include "Guards.h"
}

TEST(VecTest, CreateAndDrop) {
    Vec v = Vec_value(4, sizeof(int));
    EXPECT_EQ(Vec_length(&v), 0);
    EXPECT_EQ(v.capacity, 4);
    EXPECT_NE(v.buffer, nullptr);

    Vec_drop(&v);
    EXPECT_EQ(v.buffer, nullptr);
    EXPECT_EQ(v.capacity, 0);
    EXPECT_EQ(v.length, 0);
}

TEST(VecTest, SetAndGet) {
    Vec v = Vec_value(2, sizeof(int));
    int x = 42;
    Vec_set(&v, 0, &x);

    int out = 0;
    Vec_get(&v, 0, &out);
    EXPECT_EQ(out, 42);

    Vec_drop(&v);
}

TEST(VecTest, RefInvalidIndex) {
    Vec v = Vec_value(2, sizeof(int));
    int x = 99;
    Vec_set(&v, 0, &x);

    // valid index
    EXPECT_NE(Vec_ref(&v, 0), nullptr);

    // invalid index
    EXPECT_EQ(Vec_ref(&v, 5), nullptr);

    Vec_drop(&v);
}

TEST(VecTest, ItemsPointer) {
    Vec v = Vec_value(3, sizeof(double));
    EXPECT_EQ(Vec_items(&v), v.buffer);
    Vec_drop(&v);
}
