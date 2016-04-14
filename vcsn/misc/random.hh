#pragma once

#include <iterator>
#include <random>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Generate a unique random device.
  std::mt19937& make_random_engine();

  /// Random selector on container, using discrete distribution.
  template <typename RandomGenerator>
  struct discrete_chooser
  {
    discrete_chooser(RandomGenerator& g)
      : gen_(g)
    {}

    template <typename Iter_weight, typename Iter>
    Iter select(Iter_weight start_w, Iter_weight end_w, Iter start)
    {
      auto dis = std::discrete_distribution<>(start_w, end_w);
      std::advance(start, dis(gen_));
      return start;
    }

    template <typename Iter_weight, typename Iter>
    Iter operator()(Iter_weight start_w, Iter_weight end_w, Iter start)
    {
      return select(start_w, end_w, start);
    }

  private:
    RandomGenerator& gen_;
  };

  /// Random selector on container, using uniform distribution.
  /// Based on https://gist.github.com/cbsmith/5538174.
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
