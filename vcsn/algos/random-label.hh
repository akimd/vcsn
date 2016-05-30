#pragma once

#include <vcsn/labelset/nullableset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/irange.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/random.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{
  /// Random label from oneset.
  template <typename RandomGenerator = std::default_random_engine>
  typename oneset::value_t
  random_label(const oneset& ls,
               RandomGenerator& = RandomGenerator())
  {
    return ls.one();
  }


  /// Random label from tupleset.
  template <typename... LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename tupleset<LabelSet...>::value_t
  random_label(const tupleset<LabelSet...>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    return random_label_(ls, gen, ls.indices);
  }


  /// Implementation detail for random label from tupleset.
  template <typename... LabelSet,
            size_t... I,
            typename RandomGenerator = std::default_random_engine>
  typename tupleset<LabelSet...>::value_t
  random_label_(const tupleset<LabelSet...>& ls,
                RandomGenerator& gen,
                detail::index_sequence<I...>)
  {
    // No need to check for the emptiness here: it will be checked in
    // each sub-labelset.
    return ls.tuple(random_label(ls.template set<I>(), gen)...);
  }


  /// Random label from wordset.
  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename wordset<GenSet>::value_t
  random_label(const wordset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    require(!ls.generators().empty(),
            "random_label: the alphabet needs at least 1 letter");
    auto dis = std::uniform_int_distribution<>(0, 5);
    auto res_label = ls.one();
    auto pick = make_random_selector(gen);
    for (auto _: detail::irange(dis(gen)))
      res_label = ls.mul(res_label, ls.value(pick(ls.generators())));
    return res_label;
  }


  /// Random label from general case such as letterset.
  template <typename LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename LabelSet::value_t
  random_label(const LabelSet& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    require(!ls.generators().empty(),
            "random_label: the alphabet needs at least 1 letter");
    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);
    return ls.value(pick(ls.generators()));
  }


  /// Random label from nullableset.
  template <typename LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename nullableset<LabelSet>::value_t
  random_label(const nullableset<LabelSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    // FIXME: the proportion should be controllable.
    auto dis = std::bernoulli_distribution(0.5);
    if (dis(gen) || ls.generators().empty())
      return ls.one();
    else
      return ls.value(random_label(*ls.labelset(), gen));
  }


  /// Random label from expressionset: limited to a single label.
  template <typename Context,
            typename RandomGenerator = std::default_random_engine>
  typename expressionset<Context>::value_t
  random_label(const expressionset<Context>& rs,
               RandomGenerator& gen = RandomGenerator())
  {
    return rs.atom(random_label(*rs.labelset(), gen));
  }
}
