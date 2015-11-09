#pragma once

#include <algorithm>

#include <vcsn/misc/dynamic_bitset.hh>

#include <vcsn/misc/wet.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*------------------------------------------.
  | minimization with Hopcrofts's algorithm.  |
  `------------------------------------------*/

  template <typename Aut>
  inline
  auto
  minimize_hopcroft(const Aut& a)
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

    auto p = partition_t{f, q - f};
    auto w = partition_t{f};

    const auto& ls = *a->labelset();
    auto generators = detail::make_vector(ls.generators());
    generators.emplace_back(ls.special());

    while (!w.empty())
      {
        auto sub_w = *begin(w);
        w.erase(sub_w);
        for (const auto l: generators)
          {
            auto x = set_t(size);
            for (auto s: sub_w)
              for (auto in: a->in(label_of(s), l))
                x.set(a->src_of(in));
            auto cpy = partition_t(p);
            for (auto y: cpy)
              {
                auto xny = x & y;
                auto x_y = y - x;
                if (!xny.empty() && !x_y.empty())
                  {
                    p.erase(y);
                    p.insert(xny);
                    p.insert(x_y);
                    if (has(w, y))
                      {
                        w.erase(y);
                        w.insert(xny);
                        w.insert(x_y);
                      }
                    else
                      w.insert(xny.size() <= x_y.size() ? xny : x_y);
                  }
              }
          }
      }
    auto res = std::vector<std::vector<state_t>>();
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

  template <typename Aut>
  inline
  auto
  cominimize_hopcroft(const Aut& a)
  {
    return transpose(minimize_hopcroft(transpose(a)));
  }

} // namespace vcsn
