#pragma once

#include <unordered_map>
#include <unordered_set>

#include <vcsn/algos/accessible.hh> // is_trim
#include <vcsn/algos/quotient.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/map.hh> // vcsn::less
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*---------------------------------------------------------.
  | minimization with Moore's algorithm: signature variant.  |
  `---------------------------------------------------------*/
  namespace detail_signature
  {
    template <typename Aut>
    class minimizer
    {
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "minimize: signature: requires Boolean weights");

      using automaton_t = Aut;

      /// Input automaton, supplied at construction time.
      const automaton_t &a_;

      using labelset_t = labelset_t_of<automaton_t>;
      const labelset_t& ls_;

      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using class_t = unsigned;
      using set_t = std::vector<state_t>;
      using state_to_class_t = std::unordered_map<state_t, class_t>;
      using class_to_set_t = std::vector<set_t>;

      constexpr static const char* me() { return "minimize-signature"; }

      /// An invalid class.
      constexpr static class_t class_invalid = -1;
      unsigned num_classes_ = 0;

      class_to_set_t class_to_set_;
      state_to_class_t state_to_class_;

    public:
      static std::ostream& print_(const set_t& ss, std::ostream& o)
      {
        o << '{';
        const char* sep = "";
        for (auto s : ss)
          {
            o << sep << s;
            sep = ", ";
          }
        return o << "}";
      }

    public:
      static std::ostream& print_(const class_to_set_t& c2ss, std::ostream& o)
      {
        const char* sep = "";
        for (unsigned i = 0; i < c2ss.size(); ++i)
          {
            o << sep << '[' << i << "] = ";
            print_(c2ss[i], o);
            sep = "\n";
          }
        return o;
      }

      // For a given state, destination states for a specific label.
      struct state_output_for_label_t
      {
        // For some unstored state.
        label_t label;
        std::vector<state_t> to_states; // Ordered.

        friend
        std::ostream&
        operator<<(std::ostream& o, const state_output_for_label_t& out)
        {
          o << "out{" << out.label << " => ";
          const char* sep = "";
          for (auto s: out.to_states)
            {
              o << sep << s;
              sep = ", ";
            }
          return o << "}";
        }
      };

      // This is sorted by label.
      using state_output_t = std::vector<state_output_for_label_t>;

    public:
      static std::ostream& print_(const state_output_t& outs, std::ostream& o)
      {
        bool first = true;
        o << '{';
        for (const auto& out: outs)
          {
            if (!first)
              o << ", ";
            o << out;
            first = false;
          }
        return o << '}';
      }

      // This structure is only useful at initialization time, when
      // sorting transitions from a given state in a canonical order.
      using label_to_states_t
        = std::map<label_t, std::vector<state_t>, vcsn::less<labelset_t>>;

      std::unordered_map<state_t, state_output_t> state_to_state_output_;

      friend class signature_hasher;
      class signature_hasher : public std::hash<state_output_t*>
      {
        const state_to_class_t& state_to_class_;
        unsigned num_classes_;
      public:
        signature_hasher(minimizer& the_minimizer,
                         size_t num_classes)
          : state_to_class_(the_minimizer.state_to_class_)
          , num_classes_(num_classes)
        {}

        size_t operator()(const state_output_t* state_output_) const noexcept
        {
          const state_output_t& state_output = *state_output_;
          size_t res = 0;
          dynamic_bitset bits(num_classes_);
          for (auto& t : state_output)
            {
              const label_t& label = t.label;
              std::hash_combine(res, label);
              // Hash the set of classes reached with label.  Of
              // course the hash must not depend on class ordering.
              bits.reset();
              for (auto s : t.to_states)
                bits.set(state_to_class_.at(s));
              std::hash_combine(res, bits);
            }
#if DEBUG
          print_(state_output, std::cerr) << " = " << res << std::endl;
#endif
          return res;
        }
      }; // class signature_hasher

      friend class signature_equal_to;
      class signature_equal_to : public std::equal_to<state_output_t*>
      {
        minimizer& minimizer_;
        const labelset_t& ls_;
        const state_to_class_t& state_to_class_;
        const size_t class_bound_;
      public:
        signature_equal_to(minimizer& the_minimizer,
                           // FIXME: remove these unless really needed
                           const labelset_t& ls,
                           const state_to_class_t& state_to_class,
                           size_t class_bound)
          : minimizer_(the_minimizer)
          , ls_(ls)
          , state_to_class_(state_to_class)
          , class_bound_(class_bound)
        {}

        bool operator()(const state_output_t *as_,
                        const state_output_t *bs_) const noexcept
        {
          const state_output_t& as = *as_;
          const state_output_t& bs = *bs_;
#if DEBUG
          print_(as, std::cerr) << " =? ";
          print_(bs, std::cerr) << " = ";
#endif
          if (as.size() != bs.size())
            {
#if DEBUG
              std::cerr << "false 1" << std::endl;
#endif
              return false;
            }

          dynamic_bitset a_bits(class_bound_), b_bits(class_bound_);
          for (auto i = as.cbegin(), i_end = as.cend(), j = bs.cbegin();
               i != i_end;
               ++i, ++j)
            {
              if (! ls_.equal(i->label, j->label))
                {
#if DEBUG
                  std::cerr << "false 2" << std::endl;
#endif
                  return false;
                }

              a_bits.reset(); b_bits.reset();
              for (auto s : i->to_states)
                a_bits.set(state_to_class_.at(s));
              for (auto s : j->to_states)
                b_bits.set(state_to_class_.at(s));
              if (a_bits != b_bits)
                {
#if DEBUG
                  std::cerr << "false 3" << std::endl;
#endif
                  return false;
                }
            }
#if DEBUG
          std::cerr << "true" << std::endl;
#endif
          return true;
        }
      }; // class signature_equal_to

      friend class signature_multimap;
      class signature_multimap
        : public std::unordered_map<state_output_t*, set_t,
                                    signature_hasher, signature_equal_to>
      {
        minimizer& minimizer_;
        const state_to_class_t& state_to_class_;
        using super_t
          = std::unordered_map<state_output_t*, set_t,
                               signature_hasher, signature_equal_to>;
      public:
        signature_multimap(minimizer& the_minimizer,
                           // FIXME: remove these unless really needed.
                           const labelset_t& ls,
                           state_to_class_t& state_to_class,
                           const size_t class_bound)
          : super_t(1,
                    signature_hasher(the_minimizer, class_bound),
                    signature_equal_to(the_minimizer,
                                       ls, state_to_class, class_bound))
          , minimizer_(the_minimizer)
          , state_to_class_(state_to_class)
        {}

        friend std::ostream& operator<<(std::ostream& o,
                                        const signature_multimap& mm)
        {
          o << '{' << incendl;
          for (const auto& o_s : mm)
            {
              print_(*o_s.first, o);
              o << " : {";
              const char* sep = "";
              for (auto s: o_s.second)
                {
                  o << sep << s << '%' << mm.state_to_class_.at(s);
                  sep = ", ";
                }
              o << '}' << iendl;
            }
          o << '}' << decendl;
          return o;
        }
      }; // class signature_multimap

      void clear()
      {
        class_to_set_.clear();
        state_to_class_.clear();
        num_classes_ = 0;
      }

      /// Make a new class with the given set of states.
      class_t make_class(set_t&& set, class_t number = class_invalid)
      {
        if (number == class_invalid)
          number = num_classes_++;

        for (auto s : set)
          state_to_class_[s] = number;

        if (number < class_to_set_.size())
          class_to_set_[number] = std::move(set);
        else
          {
            assert(number == class_to_set_.size());
            class_to_set_.emplace_back(std::move(set));
          }

        return number;
      }

    public:
      minimizer(const Aut& a)
        : a_(a)
        , ls_(*a_->labelset())
      {
        require(is_trim(a_), me(), ": input must be trim");

        // Fill state_to_state_output.
        for (auto s : a_->all_states())
          {
            // Get the out-states from s, by label:
            label_to_states_t label_to_states;
            for (auto t : a_->all_out(s))
              label_to_states[a_->label_of(t)].emplace_back(a_->dst_of(t));

            // Associate this information to s, as a vector sorted by label:
            state_output_t& state_output = state_to_state_output_[s];
            for (auto& l_ss : label_to_states)
              {
                std::sort(l_ss.second.begin(), l_ss.second.end());
                state_output.emplace_back(state_output_for_label_t{l_ss.first,
                      std::move(l_ss.second)});
              }
          }
      }

      /// Build the initial classes, and split until fix point.
      void build_classes_()
      {
        // Don't even bother to split between final and non-final
        // states, this initialization is useless.
        std::unordered_set<class_t> classes;
        {
          const auto& all = a_->all_states();
          classes.insert(make_class(set_t{std::begin(all), std::end(all)}));
        }

        bool go_on;
        do
          {
            go_on = false;
#if DEBUG
            print_(class_to_set_, std::cerr) << std::endl;
#endif
            for (auto i = std::begin(classes), end = std::end(classes);
                 i != end;
                 /* nothing. */)
              {
                auto c = *i;
                const set_t& c_states = class_to_set_.at(c);

                if (c_states.size() < 2)
                  {
                    i = classes.erase(i);
                    continue;
                  }

                // Try to find distinguishable states in c_states:
                auto sig_to_state = signature_multimap{*this,
                                                       ls_, state_to_class_,
                                                       num_classes_};
                for (auto s : c_states)
                  {
#if DEBUG
                    std::cerr << "class %" << c << " state: " << s << ' ';
                    print_(state_to_state_output_[s], std::cerr) << std::endl;
#endif
                    sig_to_state[&state_to_state_output_[s]].emplace_back(s);
                  }
#if DEBUG
                std::cerr << "sig_to_state: " << sig_to_state
                          << std::endl;
#endif
                if (2 <= sig_to_state.size())
                  {
                    go_on = true;
                    i = classes.erase(i);
                    for (auto p: sig_to_state)
                      {
                        class_t c2 = make_class(std::move(p.second), c);
                        classes.insert(c2);
                        c = class_invalid;
                      }
                  }
                else
                  ++i;
              } // for on classes
          }
        while (go_on);
      }

      /// The minimized automaton.
      partition_automaton<automaton_t> operator()()
      {
        build_classes_();
        return quotient(a_, class_to_set_);
      }
    };

  } // detail_signature::

  template <typename Aut>
  inline
  auto
  minimize_signature(const Aut& a)
    -> partition_automaton<Aut>
  {
    detail_signature::minimizer<Aut> minimize(a);
    return minimize();
  }

} // namespace vcsn
