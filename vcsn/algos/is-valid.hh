#pragma once

#include <type_traits>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/direction.hh>

#include <vcsn/algos/epsilon-remover.hh>

namespace vcsn
{

  /*----------------.
  | is_valid(aut).  |
  `----------------*/

  namespace detail
  {

    /// Copy of \a aut, with absolute values.
    /// Templated to avoid useless instantiations.
    template <Automaton Aut>
    fresh_automaton_t_of<Aut>
    absval(const Aut& aut)
    {
      auto res = copy(aut);
      // Apply absolute value to the weight of each transition.
      const auto& ws = *aut->weightset();
      for (auto t: transitions(res))
        res->set_weight(t, ws.abs(res->weight_of(t)));
      return res;
    }

    /// Whether proper_here(aut) succeeds.
    /// Destroys aut.
    template <Automaton Aut>
    bool is_properable(Aut&& aut)
    {
      try
        {
          detail::epsilon_remover<Aut> remover(aut, true);
          remover();
          return true;
        }
      catch (const std::runtime_error&)
        {
          return false;
        }
    }


    template <Automaton Aut, bool has_one = context_t_of<Aut>::has_one()>
    class is_valider
    {
    public:
      using automaton_t = std::remove_cv_t<Aut>;
      using weightset_t = weightset_t_of<automaton_t>;

      /** @brief Whether an automaton is valid.

         The behavior of this method depends on the star_status of
         the weight_set:

         -- starrable: return true;
         -- tops: copy the input and return the result of proper on the copy;
         -- non_starrable: return true iff the automaton is epsilon-acyclic
         WARNING: for weight_sets with zero divisor, should test whether
         the weight of every simple circuit is zero;
         -- absval: build a copy of the input where each weight
         is replaced by its absolute value and return the result
         of proper on the copy.

         @param aut The tested automaton
         @return true iff the automaton is valid
      */

      static bool is_valid(const automaton_t& aut)
      {
        return is_valid_<weightset_t::star_status()>(aut);
      }

    private:
      template <star_status_t Status>
      static
      std::enable_if_t<Status == star_status_t::TOPS, bool>
      is_valid_(const automaton_t& aut)
      {
        return (is_proper(aut)
                || is_eps_acyclic(aut)
                || is_properable(copy(aut)));
      }

      template <star_status_t Status>
      static
      std::enable_if_t<Status == star_status_t::ABSVAL, bool>
      is_valid_(const automaton_t& aut)
      {
        return (is_proper(aut)
                || is_eps_acyclic(aut)
                || is_properable(absval(aut)));
      }

      template <star_status_t Status>
      static
      std::enable_if_t<Status == star_status_t::STARRABLE, bool>
      is_valid_(const automaton_t&)
      {
        return true;
      }

      template <star_status_t Status>
      static
      std::enable_if_t<Status == star_status_t::NON_STARRABLE, bool>
      is_valid_(const automaton_t& aut)
      {
        return is_proper(aut) || is_eps_acyclic(aut);
      }
    };

    template <Automaton Aut>
    class is_valider<Aut, false>
    {
      using automaton_t = std::remove_cv_t<Aut>;
    public:
      static constexpr bool is_valid(const automaton_t&)
      {
        return true;
      }
    };

  }


  template <Automaton Aut>
  inline
  bool is_valid(const Aut& aut)
  {
    return detail::is_valider<Aut>::is_valid(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_valid(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_valid(strip(a));
      }
    }
  }
} // namespace vcsn
