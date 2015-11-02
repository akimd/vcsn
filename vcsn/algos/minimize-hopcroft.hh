#pragma once

#include <algorithm>

#include <vcsn/misc/dynamic_bitset.hh>

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
    using set_t = dynamic_bitset;
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
            for (auto s = 0U; s < size; s++)
              if (sub_w.test(s))
                for (auto in: a->in(s, l))
                  x.set(a->src_of(in));
            auto cpy = partition_t(p);
            for (auto y: cpy)
              {
                auto xny = x & y;
                auto x_y = y - x;
                if (xny.any() && x_y.any())
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
                      w.insert(xny.count() <= x_y.count() ? xny : x_y);
                  }
              }
          }
      }
    auto res = std::vector<std::vector<state_t>>();
    std::transform(begin(p), end(p), std::back_inserter(res),
        [size](const set_t& s)
        {
          auto tab = std::vector<state_t>();
          tab.reserve(s.count());
          for (auto i = 0U; i < size; i++)
            if (s.test(i))
              tab.push_back(i);
          return tab;
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
