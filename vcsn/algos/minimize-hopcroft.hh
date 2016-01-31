#pragma once

#include <algorithm>

#include <vcsn/misc/attributes.hh>
#include <vcsn/algos/is-free-boolean.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/wet.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/polynomialset.hh>
#include <vcsn/algos/quotient.hh>

namespace vcsn
{

  /*------------------------------------------.
  | minimization with Hopcrofts's algorithm.  |
  `------------------------------------------*/

  /// Request for Hopcroft implementation of minimize (B and free).
  struct hopcroft_tag {};

  template <Automaton Aut>
  std::enable_if_t<is_free_boolean<Aut>(), quotient_t<Aut>>
  minimize(const Aut& a, hopcroft_tag)
  {
    using state_t = state_t_of<Aut>;
    using stateset_t = stateset<Aut>;
    require(std::is_same<weightset_t_of<Aut>, b>::value,
            "hopcroft: require boolean weightset");
    auto ctx = make_context(stateset_t(a), *a->weightset());
    auto ps = detail::make_polynomialset<decltype(ctx),
                                         wet_kind_t::bitset>(ctx);
    using set_t = typename decltype(ps)::value_t;
    using partition_t = std::set<set_t>;

    unsigned size = a->all_states().back() + 1;

    auto f = set_t(size);
    f.set(a->post());

    auto q = set_t(size);
    for (auto s: a->all_states())
      q.set(s);
    q.erase(a->post());

    auto p = partition_t{f, q};
    auto w = partition_t{f};

    const auto& ls = *a->labelset();
    auto generators = detail::make_vector(ls.generators());
    generators.emplace_back(ls.special());

    while (!w.empty())
      {
        auto sub_w = std::move(*begin(w));
        w.erase(begin(w));
        for (const auto l: generators)
          {
            auto x = set_t(size);
            for (auto s: sub_w)
              for (auto t: in(a, label_of(s), l))
                x.set(a->src_of(t));
            auto p2 = partition_t(p);
            for (auto y: p2)
              {
                auto xny = x & y;
                auto x_y = y - x;
                if (!xny.empty() && !x_y.empty())
                  {
                    auto it = w.find(y);
                    if (it != end(w))
                      {
                        w.erase(it);
                        w.insert(xny);
                        w.insert(x_y);
                      }
                    else
                      w.insert(xny.size() <= x_y.size() ? xny : x_y);
                    p.erase(y);
                    p.insert(std::move(xny));
                    p.insert(std::move(x_y));
                  }
              }
          }
      }
    auto res = std::vector<std::vector<state_t>>{};
    std::transform(begin(p), end(p), std::back_inserter(res),
        [](const set_t& set)
        {
          auto res = std::vector<state_t>();
          res.reserve(set.size());
          for (auto s: set)
            res.push_back(label_of(s));
          return res;
        });
    return quotient(a, res);
  }

  namespace dyn
  {
    namespace detail
    {
      template <Automaton Aut>
      ATTRIBUTE_NORETURN
      std::enable_if_t<!is_free_boolean<Aut>(), quotient_t<Aut>>
      minimize(const Aut&, hopcroft_tag)
      {
        raise("minimize: invalid algorithm"
              " (non-Boolean or non-free labelset):",
              " hopcroft");
      }
    }
  }
} // namespace vcsn
