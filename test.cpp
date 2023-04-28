#include "binary_search.hpp"

struct Standard
{
    template<typename It, typename T, typename C>
    auto operator()(It begin, It end, const T & val, C && cmp)
    {
        return std::lower_bound(begin, end, val, cmp);
    }
};
struct Branchless
{
    template<typename It, typename T, typename C>
    auto operator()(It begin, It end, const T & val, C && cmp)
    {
        return branchless_lower_bound(begin, end, val, cmp);
    }
};

#ifdef ENABLE_GTEST
#include <gtest/gtest.h>

template<typename BinarySearch, typename Cmp>
void test_all(Cmp && cmp, int limit = 64)
{
    std::vector<int> sorted;
    auto found = BinarySearch()(sorted.begin(), sorted.end(), 0, cmp);
    ASSERT_EQ(sorted.end(), found);
    for (int i = 0; i < limit; ++i)
    {
        sorted.push_back(i);
        found = BinarySearch()(sorted.begin(), sorted.end(), -1, cmp);
        ASSERT_EQ(0, *found) << "i: " << i;
        for (int j = 0; j <= i; ++j)
        {
            found = BinarySearch()(sorted.begin(), sorted.end(), j, cmp);
            ASSERT_EQ(j, *found) << "i: " << i << ", j: " << j;
        }
        found = BinarySearch()(sorted.begin(), sorted.end(), i + 1, cmp);
        ASSERT_EQ(sorted.end(), found) << "i: " << i;
    }
}
template<typename BinarySearch>
void test_all(int limit = 64)
{
    test_all<Branchless>(std::less<>{}, limit);
}

struct CountingCompare
{
    template<typename T, typename U>
    bool operator()(T && l, U && r)
    {
        ++count;
        return std::less<>{}(l, r);
    }

    int & count;
};

template<typename BinarySearch>
void test_counts(int limit = 64)
{
    int count = 0;
    CountingCompare cmp{count};
    std::vector<int> sorted;
    auto found = BinarySearch()(sorted.begin(), sorted.end(), 0, cmp);
    ASSERT_EQ(sorted.end(), found);
    ASSERT_EQ(0, count);
    for (int i = 0; i < limit; ++i)
    {
        sorted.push_back(i);
        int compare_limit = 65 - std::countl_zero(bit_ceil(i));
        count = 0;
        found = BinarySearch()(sorted.begin(), sorted.end(), -1, cmp);
        ASSERT_EQ(0, *found) << "i: " << i;
        int actual_count = count;
        count = 0;
        Standard()(sorted.begin(), sorted.end(), -1, cmp);
        ASSERT_LE(actual_count, compare_limit) << "count: " << count << ", actual_count: " << actual_count << ", i: " << i;
        for (int j = 0; j <= i; ++j)
        {
            count = 0;
            found = BinarySearch()(sorted.begin(), sorted.end(), j, cmp);
            ASSERT_EQ(j, *found) << "i: " << i << ", j: " << j;
            actual_count = count;
            count = 0;
            Standard()(sorted.begin(), sorted.end(), j, cmp);
            ASSERT_LE(actual_count, compare_limit) << "count: " << count << ", actual_count: " << actual_count << ", i: " << i << ", j: " << j;
        }
        count = 0;
        found = BinarySearch()(sorted.begin(), sorted.end(), i + 1, cmp);
        ASSERT_EQ(sorted.end(), found) << "i: " << i;
        actual_count = count;
        count = 0;
        Standard()(sorted.begin(), sorted.end(), i + 1, cmp);
        ASSERT_LE(actual_count, compare_limit) << "count: " << count << ", actual_count: " << actual_count << ", i: " << i;
    }
}

TEST(binary_search, all_shar_odd)
{
    int count = 0;
    CountingCompare cmp{count};
    test_all<Branchless>(cmp);
    ASSERT_EQ(14286, count);
}

TEST(binary_search, counts)
{
    test_counts<Branchless>();
}

TEST(binary_search, half_shar_odd)
{
    int count = 0;
    CountingCompare cmp{count};
    test_all<Branchless>(cmp, 1);
    EXPECT_EQ(3, count);
    count = 0;
    test_all<Branchless>(cmp, 2);
    EXPECT_EQ(11, count);
    count = 0;
    test_all<Branchless>(cmp, 3);
    EXPECT_EQ(24, count);
    count = 0;
    test_all<Branchless>(cmp, 4);
    EXPECT_EQ(42, count);
}

TEST(binary_search, all_lower_bound)
{
    int count = 0;
    CountingCompare cmp{count};
    test_all<Standard>(cmp);
    ASSERT_EQ(11835, count);
}

TEST(binary_search, half_lower_bound)
{
    int count = 0;
    CountingCompare cmp{count};
    test_all<Standard>(cmp, 1);
    EXPECT_EQ(3, count);
    count = 0;
    test_all<Standard>(cmp, 2);
    EXPECT_EQ(10, count);
    count = 0;
    test_all<Standard>(cmp, 3);
    EXPECT_EQ(20, count);
    count = 0;
    test_all<Standard>(cmp, 4);
    EXPECT_EQ(35, count);
}

#endif

#include <iostream>

#ifdef ENABLE_GTEST
int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#else
int main()
{
    std::vector<float> floats;
    for (int f = 0.0f; f < 100.0f; ++f)
    {
        floats.push_back(f);
    }
    for (float f = -1.0f; f <= 100.0f; ++f)
    {
        auto found0 = std::lower_bound(floats.begin(), floats.end(), f);
        auto found1 = branchless_lower_bound(floats.begin(), floats.end(), f);
        if (found0 != found1)
            std::cout << "Error " << f << std::endl;
        found0 = std::lower_bound(floats.begin(), floats.end(), f + 0.5f);
        found1 = branchless_lower_bound(floats.begin(), floats.end(), f + 0.5f);
        if (found0 != found1)
            std::cout << "Error " << (f + 0.5f) << std::endl;
    }
}
#endif
