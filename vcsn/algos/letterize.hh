#pragma once

#include <iterator>

#include <vcsn/algos/proper.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // context_t_of
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/labelset.hh> // detail::make_letterized
#include <vcsn/misc/algorithm.hh> // detail::back

namespace vcsn
{

  /*------------------------.
  | letterize(automaton).   |
  `------------------------*/
  namespace detail
  {
    /// From an automaton, the corresponding automaton with a non-word labelset.
    template <Automaton AutIn, Automaton AutOut>
    class letterizer
    {
    public:
      using in_automaton_t = AutIn;
      using in_ctx_t = context_t_of<in_automaton_t>;
      using in_state_t = state_t_of<in_automaton_t>;
      using in_labelset_t = labelset_t_of<in_automaton_t>;
      using in_label_t = typename in_labelset_t::value_t;
      using in_transition_t = transition_t_of<in_automaton_t>;

      using out_automaton_t = AutOut;
      using out_ctx_t = context_t_of<out_automaton_t>;
      using out_state_t = state_t_of<out_automaton_t>;
      using out_labelset_t = labelset_t_of<out_automaton_t>;
      using out_label_t = typename out_labelset_t::value_t;
      using out_transition_t = transition_t_of<out_automaton_t>;

      // map in_state_t -> out_state_t
      using map_t = std::vector<out_state_t>;

      letterizer(const in_automaton_t& in_aut, const out_labelset_t& ls)
        : in_aut_(in_aut)
        , out_aut_(make_mutable_automaton(out_ctx_t{ls, *in_aut->weightset()}))
        , state_map_(detail::back(in_aut->all_states()) + 1)
      {}

      out_automaton_t letterize()
      {
        auto in_ls = in_aut_->labelset();
        auto out_ws = out_aut_->weightset();
        // Copy the states, and setup the map
        state_map_[in_aut_->pre()] = out_aut_->pre();
        state_map_[in_aut_->post()] = out_aut_->post();
        for (auto st : in_aut_->states())
          state_map_[st] = out_aut_->new_state();

        for (auto st : in_aut_->all_states())
          for (auto tr : all_out(in_aut_, st))
            {
              auto letters = in_ls->letters_of_padded(in_aut_->label_of(tr),
                                                      out_labelset_t::one());
              auto it = letters.begin();
              if (it != letters.end())
              {
                auto src = state_map_[st];
                auto dst = std::next(it) != letters.end()
                         ? out_aut_->new_state()
                         : state_map_[in_aut_->dst_of(tr)];
                out_aut_->new_transition(src, dst,
                                        *it, in_aut_->weight_of(tr));
                src = dst;
                for (++it; it != letters.end(); ++it)
                {
                  dst = std::next(it) == letters.end()
                      ? state_map_[in_aut_->dst_of(tr)]
                      : out_aut_->new_state();
                  out_aut_->new_transition(src, dst, *it, out_ws->one());
                  src = dst;
                }
              }
              else
                out_aut_->new_transition(state_map_[st],
                                        state_map_[in_aut_->dst_of(tr)],
                                        out_labelset_t::one(),
                                        in_aut_->weight_of(tr));
            }

        return std::move(out_aut_);
      }

    protected:
      in_automaton_t in_aut_;
      out_automaton_t out_aut_;
      map_t state_map_;
    };


    template <Automaton Aut>
    using letterized_ls = letterized_traits<labelset_t_of<Aut>>;

    /// Letterize an automaton whose type is not letterized already.
    template <Automaton Aut>
    std::enable_if_t<!is_letterized_t<labelset_t_of<Aut>>{},
                      mutable_automaton<letterized_context<context_t_of<Aut>>>>
    letterize(const Aut& aut)
    {
      using res_t = mutable_automaton<letterized_context<context_t_of<Aut>>>;
      auto lt = letterizer<Aut, res_t>{aut,
                                       make_letterized(*aut->labelset())};
      return lt.letterize();
    }

    /// Letterize an automaton whose type is letterized: do nothing.
    template <Automaton Aut>
    std::enable_if_t<is_letterized_t<labelset_t_of<Aut>>{},
                      const Aut&>
    letterize(const Aut& aut)
    {
      return aut;
    }
  }

  /*------------------------.
  | letterize(automaton).   |
  `------------------------*/

  /// Split the word transitions in the input automaton into letter ones.
  ///
  /// \param[in] aut        the automaton
  /// \returns              the letterized automaton
  template <Automaton Aut>
  auto
  letterize(const Aut& aut)
    -> decltype(detail::letterize(aut))
  {
    return detail::letterize(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton letterize(const automaton& aut)
      {
        return make_automaton(::vcsn::letterize(aut->as<Aut>()));
      }
    }
  }

  /*----------------------------.
  | is_letterized(automaton).   |
  `----------------------------*/

  namespace detail
  {
    template <Automaton Aut>
    std::enable_if_t<!is_letterized_t<labelset_t_of<Aut>>{}, bool>
    is_letterized(const Aut& aut)
    {
      auto ls = aut->labelset();
      for (auto t : transitions(aut))
      {
        auto it = ls->letters_of_padded(aut->label_of(t),
                                        letterized_ls<Aut>::labelset_t::one());
        // size is not 0 or 1, then it's a word
        // we can't use size, as it's not defined for zip_iterators
        if (it.begin() != it.end() && ++(it.begin()) != it.end())
          return false;
      }
      return true;
    }

    template <Automaton Aut>
    std::enable_if_t<is_letterized_t<labelset_t_of<Aut>>{}, bool>
    is_letterized(const Aut&)
    {
      return true;
    }
  }

  /// Check if the transitions are all letters.
  ///
  /// \param[in] aut        the automaton
  /// \returns              whether the transitions are letters
  template <Automaton Aut>
  bool
  is_letterized(const Aut& aut)
  {
    return detail::is_letterized(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_letterized(const automaton& aut)
      {
        return ::vcsn::is_letterized(aut->as<Aut>());
      }
    }
  }

  /*-----------.
  | realtime.  |
  `-----------*/

  /// Split the word transitions in the input automaton into letter ones, and
  /// remove the spontaneous transitions
  ///
  /// \param[in] aut        the automaton
  /// \returns              the realtime automaton
  template <Automaton Aut>
  auto
  realtime(const Aut& aut)
    -> decltype(proper(::vcsn::letterize(aut)))
  {
    return proper(::vcsn::letterize(aut));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton realtime(const automaton& aut)
      {
        return make_automaton(::vcsn::realtime(aut->as<Aut>()));
      }
    }
  }

  /*--------------.
  | is_realtime.  |
  `--------------*/

  /// Check if the automaton is realtime, i.e. it is letterized and proper.
  ///
  /// \param[in] aut        the automaton
  /// \returns              whether the automaton is realtime
  template <Automaton Aut>
  bool
  is_realtime(const Aut& aut)
  {
    return ::vcsn::is_letterized(aut) && is_proper(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_realtime(const automaton& aut)
      {
        return ::vcsn::is_realtime(aut->as<Aut>());
      }
    }
  }
} // namespace vcsn
