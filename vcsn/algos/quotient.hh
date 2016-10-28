#pragma once

#include <algorithm> // min_element.
#include <unordered_map>

#include <boost/range/algorithm/min_element.hpp>
#include <boost/range/algorithm/sort.hpp>

#include <vcsn/core/partition-automaton.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(quotient);

  namespace detail
  {
    /// Apply a quotient onto an automaton: fuse equivalent states.
    ///
    /// \tparam Aut  the type of the input automaton
    template <Automaton Aut>
    class quotienter
    {
    public:
      using automaton_t = Aut;
      using quotient_t = partition_automaton_t<automaton_t>;

      template <typename Ctx = context_t_of<Aut>>
      using fresh_automaton_t = fresh_automaton_t_of<automaton_t, Ctx>;

      using origins_t = origins_t_of<quotient_t>;

      using class_t = unsigned;
      using state_t = state_t_of<automaton_t>;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;
      using class_to_state_t = std::vector<state_t>;

      /// \param class_to_set  The equivalence classes.
      ///             Might be modified to put the states with the
      ///             smallest ID first in their class.
      quotienter(class_to_set_t& class_to_set)
        : class_to_set_(class_to_set)
        , num_classes_(class_to_set_.size())
      {
        sort_classes_();
      }

      /// Sort the classes.
      ///
      /// This step, which is "useless" in that the result would be
      /// correct anyway, just ensures that the classes are numbered
      /// after their states: classes are sorted by the smallest of
      /// their state ids.
      void sort_classes_()
      {
        // For each class, put its smallest numbered state first
        // (set_t is not a std::set, it's a vector!).  We don't need
        // to fully sort.
        for (unsigned c = 0; c < num_classes_; ++c)
            std::swap(class_to_set_[c][0],
                      *boost::min_element(class_to_set_[c]));

        // Sort class numbers by smallest state number.
        boost::sort(class_to_set_,
                    [](const set_t& lhs, const set_t& rhs)
                    {
                      return lhs[0] < rhs[0];
                    });
      }

      /// Build the resulting automaton.
      /// \param aut   the input automaton to quotient
      quotient_t operator()(const automaton_t& aut)
      {
        state_to_class_t state_to_class;
        for (unsigned c = 0; c < num_classes_; ++c)
          for (auto s: class_to_set_[c])
            state_to_class[s] = c;

        auto origins = origins_t{};
        auto res = make_fresh_automaton(aut);
        auto class_to_res_state = class_to_state_t(num_classes_);
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            const std::vector<state_t>& set = class_to_set_[c];
            state_t s = set[0];
            class_to_res_state[c]
              = s == aut->pre()  ? res->pre()
              : s == aut->post() ? res->post()
              : res->new_state();
            origins[class_to_res_state[c]].insert(begin(set), end(set));
          }
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            // Copy the outgoing transitions of the first state of the
            // class in the result.
            state_t s = class_to_set_[c][0];
            state_t src = class_to_res_state[c];
            for (auto t : all_out(aut, s))
              {
                state_t d = aut->dst_of(t);
                state_t dst = class_to_res_state[state_to_class[d]];
                res->add_transition(src, dst,
                                    aut->label_of(t), aut->weight_of(t));
              }
          }

        auto out = make_partition_automaton(res, aut, origins);
        out->properties().update(quotient_ftag{});
        return out;
      }

    private:
      class_to_set_t& class_to_set_;
      unsigned num_classes_;
    };
  }

  /// The return type when calling quotient on Aut.
  template <Automaton Aut>
  using quotient_t = partition_automaton_t<Aut>;

  template <Automaton Aut>
  auto
  quotient(const Aut& a,
           typename detail::quotienter<Aut>::class_to_set_t& cs)
    -> quotient_t<Aut>
  {
    auto quotient = detail::quotienter<Aut>{cs};
    return quotient(a);
  }

} // namespace vcsn
