#include <benchmark/benchmark.h>

static void BenchHelloWorld(benchmark::State& state)
{
    int value = 42;
    for (auto _ : state)
    {
        value += 1;
        benchmark::DoNotOptimize(value);
    }
}

BENCHMARK(BenchHelloWorld);
BENCHMARK_MAIN();
