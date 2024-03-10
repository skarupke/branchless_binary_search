#include <benchmark/benchmark.h>
#include <algorithm>
#include <concepts>
#include <vector>
#include <random>
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


template<typename T>
requires std::is_arithmetic_v<T>
static std::vector<T> gen_rand_vec(size_t sz) {
    std::vector<T> vec(sz);
    std::random_device rd;
    std::mt19937 gen(rd());

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        for (size_t i = 0; i < sz; ++i) {
            T val = dist(gen);
            vec[i] = val * val; // Squaring to skew the distribution
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        for (size_t i = 0; i < sz; ++i) {
            T val = dist(gen);
            vec[i] = val * val; // Squaring to skew the distribution
        }
    }

    std::sort(vec.begin(), vec.end());
    return vec;
}

template<typename BinarySearch>
static void bench_bs(benchmark::State &state) {
  auto bench_vec = gen_rand_vec<int64_t>(state.range(0));
  auto bs_alg = BinarySearch{};

  auto search_value  = 0LL;

  for (auto _ : state) {
    search_value = (search_value + 1) ^ (uintptr_t)(bench_vec.data());
    auto find_idx = bs_alg(bench_vec.begin(), bench_vec.end(), search_value, std::less<>{});
    benchmark::DoNotOptimize(find_idx);
  }
}

BENCHMARK_TEMPLATE(bench_bs, Standard)->RangeMultiplier(4)->Range(128, 1<<23);
BENCHMARK_TEMPLATE(bench_bs, Branchless)->RangeMultiplier(4)->Range(128, 1<<23);

BENCHMARK_MAIN();
