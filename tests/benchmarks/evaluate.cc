// master:
//
// $ v run ./tests/benchmarks/evaluate
// Run on (8 X 2600 MHz CPU s)
// 2017-11-
// -----------------------------------------------------
// Benchmark              Time           CPU Iterations
// -----------------------------------------------------
// BM_evaluate       462214 ns     458017 ns       1205

// propcache:
//
// $ v run ./tests/benchmarks/evaluate
// Run on (8 X 2600 MHz CPU s)
// 2017-11-
// -----------------------------------------------------
// Benchmark              Time           CPU Iterations
// -----------------------------------------------------
// BM_evaluate       513979 ns     505466 ns       1155

#include <benchmark/benchmark.h>
//#include <gperftools/profiler.h>

#include <vcsn/alphabets/char.hh>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/weightset/b.hh>

#include <vcsn/algos/de-bruijn.hh>
#include <vcsn/algos/evaluate.hh>

static void BM_evaluate(benchmark::State& state)
{
  using namespace vcsn;

  using ls_t = letterset<set_alphabet<char_letters>>;
  using ws_t = b;
  const auto ctx = context<ls_t, ws_t>{{{'a', 'b', 'c'}}, {}};
  const auto aut = vcsn::de_bruijn(ctx, state.range(0));
  const auto word = std::string(state.range(0) + 1, 'a');

  if (!vcsn::evaluate(aut, word))
    std::abort();

  for (auto _ : state)
    benchmark::DoNotOptimize(vcsn::evaluate(aut, word));
}
BENCHMARK(BM_evaluate)
    ->Args({1000})
    ->Args({2000})
    ->Args({5000})
    ->Args({10000});

BENCHMARK_MAIN();

// CXXFLAGS: -lbenchmark
