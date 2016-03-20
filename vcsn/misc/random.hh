#pragma once

#include <iterator>
#include <random>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Generate a unique random device.
  std::mt19937& make_random_engine();

  // Based on https://gist.github.com/cbsmith/5538174.
  template <typename RandomGenerator>
  struct random_selector
  {
    random_selector(RandomGenerator& g)
      : gen_(g)
    {}

    /// A randomly selected iterator in [start, end).
    template <typename Iter>
    Iter select(Iter start, Iter end)
    {
      auto dis
        = std::uniform_int_distribution<>(0, std::distance(start, end) - 1);
      std::advance(start, dis(gen_));
      return start;
    }

    /// A randomly selected iterator in [start, end).
    template <typename Iter>
    Iter operator()(Iter start, Iter end)
    {
      return select(start, end);
    }

    /// A randomly selected member of \a c.
    template <typename Container>
    auto operator()(const Container& c) -> decltype(*std::begin(c))
    {
      return *select(std::begin(c), std::end(c));
    }

    /// A randomly selected member of \a c.  Remove it from c.
    template <typename Container>
    auto pop(Container& c) -> typename Container::value_type
    {
      auto i = select(std::begin(c), std::end(c));
      auto res = *i;
      c.erase(i);
      return res;
    }

  private:
    RandomGenerator& gen_;
  };

  template <typename RandomGenerator>
  random_selector<RandomGenerator>
  make_random_selector(RandomGenerator& g)
  {
    return g;
  }
}
