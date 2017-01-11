#pragma once

#include <algorithm>

#include <vcsn/algos/is-free.hh>
#include <vcsn/algos/minimize-brzozowski.hh>
#include <vcsn/algos/quotient.hh>
#include <vcsn/labelset/stateset.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/misc/wet.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  struct minimize_ftag; // Defined in minimize-brzozowski.hh

  /*------------------------------------------.
  | minimization with Hopcrofts's algorithm.  |
  `------------------------------------------*/

  /// Request for Hopcroft implementation of minimize (B and free).
  struct hopcroft_tag {};

  template <Automaton Aut>
  std::enable_if_t<is_letterized_boolean<Aut>(), quotient_t<Aut>>
  minimize(const Aut& a, hopcroft_tag)
  {
    require(is_free(a),
            "minimize: invalid algorithm"
            " (non-free automaton):",
            " hopcroft");
    using state_t = state_t_of<Aut>;
    using stateset_t = stateset<Aut>;
    auto ctx = make_context(stateset_t(a), *a->weightset());
    auto ps = detail::make_polynomialset<decltype(ctx),
                                         wet_kind_t::bitset>(ctx);
    using set_t = typename decltype(ps)::value_t;
    using partition_t = std::set<set_t>;

    auto size = states_size(a);

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
    auto out = quotient(a, res);
    out->properties().update(minimize_ftag{});
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      template <Automaton Aut>
      ATTRIBUTE_NORETURN
      std::enable_if_t<!is_letterized_boolean<Aut>(), quotient_t<Aut>>
      minimize(const Aut&, hopcroft_tag)
      {
        raise("minimize: invalid algorithm"
              " (non-Boolean or non-free automaton):",
              " hopcroft");
      }
    }
  }
} // namespace vcsn
