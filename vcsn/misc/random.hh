#pragma once

#include <random>
#include <iterator>

#include <vcsn/misc/attributes.hh>

namespace vcsn
{

  // Based on https://gist.github.com/cbsmith/5538174.
  template <typename RandomGenerator = std::default_random_engine>
  struct random_selector
  {
    random_selector(const RandomGenerator& g = RandomGenerator())
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
    RandomGenerator gen_;
  };


  template <typename RandomGenerator = std::default_random_engine>
  struct random_selector<RandomGenerator>
  make_random_selector(const RandomGenerator& g)
    ATTRIBUTE_PURE;

  template <typename RandomGenerator>
  struct random_selector<RandomGenerator>
  make_random_selector(const RandomGenerator& g)
  {
    return g;
  }

} // !vcsn
