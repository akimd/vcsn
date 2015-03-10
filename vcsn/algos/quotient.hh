#pragma once

#include <algorithm> // min_element.
#include <unordered_map>

#include <vcsn/core/partition-automaton.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  namespace detail
  {
    /// Apply a quotient onto an automaton: fuse equivalent states.
    template <typename Aut>
    class quotienter
    {
    public:
      using automaton_t = Aut;
      using partition_automaton_t = partition_automaton<automaton_t>;

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
                      *std::min_element(begin(class_to_set_[c]),
                                        end(class_to_set_[c])));

        // Sort class numbers by smallest state number.
        std::sort(begin(class_to_set_), end(class_to_set_),
                  [](const set_t& lhs, const set_t& rhs) -> bool
                  {
                    return lhs[0] < rhs[0];
                  });
      }

      /// Build the resulting automaton.
      /// \param aut   the input automaton to quotient
      partition_automaton_t operator()(const automaton_t& aut)
      {
        state_to_class_t state_to_class;
        for (unsigned c = 0; c < num_classes_; ++c)
          for (auto s: class_to_set_[c])
            state_to_class[s] = c;

        auto res = make_shared_ptr<partition_automaton_t>(aut);
        auto class_to_res_state = class_to_state_t(num_classes_);
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            const std::vector<state_t>& set = class_to_set_[c];
            state_t s = set[0];
            class_to_res_state[c]
              = s == aut->pre()  ? res->pre()
              : s == aut->post() ? res->post()
              : res->new_state(set);
          }
        for (unsigned c = 0; c < num_classes_; ++c)
          {
            // Copy the transitions of the first state of the class in
            // the result.
            state_t s = class_to_set_[c][0];
            state_t src = class_to_res_state[c];
            for (auto t : aut->all_out(s))
              {
                state_t d = aut->dst_of(t);
                state_t dst = class_to_res_state[state_to_class[d]];
                res->add_transition(src, dst,
                                    aut->label_of(t), aut->weight_of(t));
              }
          }
        return res;
      }

    private:
      class_to_set_t& class_to_set_;
      unsigned num_classes_;
    };
  } // detail::

  template <typename Aut>
  inline
  auto
  quotient(const Aut& a,
           typename detail::quotienter<Aut>::class_to_set_t& cs)
    -> partition_automaton<Aut>
  {
    detail::quotienter<Aut> quotient(cs);
    return quotient(a);
  }

} // namespace vcsn
