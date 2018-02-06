#include <benchmark/benchmark.h>

#include <vcsn/alphabets/char.hh>
#include <vcsn/labelset/letterset.hh>

static void BM_letters_of(benchmark::State& state)
{
  using namespace vcsn;

  using ls_t = letterset<set_alphabet<char_letters>>;
  const auto ls = ls_t{{'a', 'b', 'c'}};
  const auto word = ls_t::word_t{std::string(256, 'a')};

  while (state.KeepRunning())
    for (const auto l : ls.letters_of(ls.delimit(word)))
      benchmark::DoNotOptimize(l);
}
BENCHMARK(BM_letters_of);

BENCHMARK_MAIN();

// CXXFLAGS: -lbenchmark
