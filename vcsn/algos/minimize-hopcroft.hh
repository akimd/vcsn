#pragma once

#include <algorithm>

#include <vcsn/misc/set.hh>
#include <vcsn/misc/vector.hh>

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
    using set_t = std::set<state_t>;
    using partition_t = std::set<set_t>;

    auto states = a->all_states();
    auto q = set_t(std::begin(states), std::end(states));
    auto f = set_t{a->post()};
    auto q_f = set_difference(q, f);

    auto p = partition_t{f, q_f};
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
            auto x = set_t();
            for (auto s: sub_w)
              for (auto in: a->in(s, l))
                x.insert(a->src_of(in));
            auto cpy = partition_t(p);
            for (auto y: cpy)
              {
                auto xny = intersection(x, y);
                auto x_y = set_difference(y, x);
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
                      {
                        if (xny.size() <= x_y.size())
                          w.insert(xny);
                        else
                          w.insert(x_y);
                      }
                  }
              }
          }
      }
    auto res = std::vector<std::vector<state_t>>();
    std::transform(begin(p), end(p), std::back_inserter(res),
        [](const set_t& s)
        {
          return std::vector<state_t>(begin(s), end(s));
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
