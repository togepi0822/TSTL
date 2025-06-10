#include <gtest/gtest.h>

#include <tgp/split_buffer.h>

using namespace tgp;

namespace {

template<class C>
void test_constructor_impl(testing::Test*) {
    using Alloc = typename C::allocator_type;
    {
        Alloc a;
        C c(8, 4, a);
        ASSERT_EQ(c.capacity(), 8);
        ASSERT_EQ(c.first_ + 4, c.begin_);
        ASSERT_EQ(c.begin_, c.end_);
        ASSERT_EQ(c.first_ + 8, c.cap_);
        ASSERT_TRUE(c.invariants());
    }
    {
        Alloc a;
        C c(8, 8, a);
        ASSERT_EQ(c.capacity(), 8);
        ASSERT_EQ(c.first_ + 8, c.begin_);
        ASSERT_EQ(c.begin_, c.end_);
        ASSERT_EQ(c.first_ + 8, c.cap_);
        ASSERT_TRUE(c.invariants());
    }
    {
        Alloc a;
        C c(0, 0, a);
        ASSERT_EQ(c.capacity(), 0);
        ASSERT_EQ(c.first_, c.begin_);
        ASSERT_EQ(c.begin_, c.end_);
        ASSERT_EQ(c.first_, c.cap_);
        ASSERT_TRUE(c.invariants());
    }
}

template<class C>
void test_construct_at_end_with_size(testing::Test*) {
    using Alloc = typename C::allocator_type;
    using T     = typename C::value_type;
    using Sz    = typename C::size_type;
    {
        Alloc a;
        C c(10, 5, a);
        c.construct_at_end(static_cast<Sz>(3));
        ASSERT_EQ(c.begin_ + 3, c.end_);
        for (auto p = c.begin_; p != c.end_; ++p)
            ASSERT_EQ(*p, T());
        ASSERT_TRUE(c.invariants());
    }
}

template<class C>
void test_construct_at_end_with_size_value(testing::Test*) {
    using Alloc = typename C::allocator_type;
    using T     = typename C::value_type;
    using Sz    = typename C::size_type;
    {
        Alloc a;
        C c(10, 5, a);
        c.construct_at_end(static_cast<Sz>(3), T{1});
        ASSERT_EQ(c.begin_ + 3, c.end_);
        for (auto p = c.begin_; p != c.end_; ++p)
            ASSERT_EQ(*p, 1);
        ASSERT_TRUE(c.invariants());
    }
}

template<class C>
void test_construct_at_end_with_sentinel(testing::Test*) {
    using Alloc = typename C::allocator_type;
    using T     = typename C::value_type;
    using Sz    = typename C::size_type;
    {
        Alloc a;
        C c(10, 5, a);
        std::vector<T> v{T{1}, T{2}, T{3}};
        c.construct_at_end(v.begin(), v.end());
        ASSERT_EQ(c.begin_ + 3, c.end_);
        for (Sz i = 0; i < 3; ++i)
            ASSERT_EQ(*(c.begin_ + i), v[i]);
        ASSERT_TRUE(c.invariants());
    }
}

template<class C>
void test_emplace_back(testing::Test*) {
    using Alloc = typename C::allocator_type;
    {
        Alloc a;
        C c(10, 5, a);
        c.emplace_back(10);
        ASSERT_EQ(c.begin_ + 1, c.end_);
        ASSERT_EQ(*(c.begin_), 10);
        ASSERT_TRUE(c.invariants());
    }
}

template<class C>
void test_emplace_front(testing::Test*) {
    using Alloc = typename C::allocator_type;
    {
        Alloc a;
        C c(10, 5, a);
        c.emplace_front(10);
        ASSERT_EQ(c.begin_ + 1, c.end_);
        ASSERT_EQ(*(c.begin_), 10);
        ASSERT_TRUE(c.invariants());
    }
}

} // end of unnamed namespace

TEST(split_buffer, constructor) {
    test_constructor_impl<split_buffer<int, std::allocator<int>&>>(this);
}

TEST(split_buffer, construct_at_end_with_size) {
    test_construct_at_end_with_size<split_buffer<int, std::allocator<int>&>>(this);
}

TEST(split_buffer, construct_at_end_with_size_value) {
    test_construct_at_end_with_size_value<split_buffer<int, std::allocator<int>&>>(this);
}

TEST(split_buffer, construct_at_end_with_sentinel) {
    test_construct_at_end_with_sentinel<split_buffer<int, std::allocator<int>&>>(this);
}

TEST(split_buffer, emplace_back) {
    test_emplace_back<split_buffer<int, std::allocator<int>&>>(this);
}

TEST(split_buffer, emplace_front) {
    test_emplace_front<split_buffer<int, std::allocator<int>&>>(this);
}