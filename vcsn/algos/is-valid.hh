#ifndef VCSN_ALGOS_IS_VALID_HH
# define VCSN_ALGOS_IS_VALID_HH

# include <stdexcept>
# include <type_traits>
# include <unordered_map>
# include <utility>
# include <vector>

# include <vcsn/algos/fwd.hh>
# include <vcsn/algos/constant-term.hh>
# include <vcsn/algos/copy.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/is-proper.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/direction.hh>

namespace vcsn
{

  /*----------------.
  | is_valid(aut).  |
  `----------------*/

  namespace detail
  {

    template <typename Aut, bool has_one = Aut::context_t::has_one()>
    class is_valider
    {
    public:
      using automaton_t = typename std::remove_cv<Aut>::type;
      using weightset_t = typename automaton_t::weightset_t;

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
      typename std::enable_if<Status == star_status_t::TOPS,
                              bool>::type
      is_valid_(const automaton_t& aut)
      {
        if (is_proper(aut) || is_eps_acyclic(aut))
          return true;
        else
          {
            automaton_t a = copy(aut);
            return in_situ_remover(a);
          }
      }

      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::ABSVAL,
                              bool>::type
      is_valid_(const automaton_t& aut)
      {
        if (is_proper(aut) || is_eps_acyclic(aut))
          return true;
        else
          {
            automaton_t a = copy(aut);
            // Apply absolute value to the weight of each transition.
            const auto& weightset = *aut.weightset();
            for (auto t: a.transitions())
              a.set_weight(t, weightset.abs(a.weight_of(t)));
            // Apply proper.
            return in_situ_remover(a);
          }
      }

      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::STARRABLE,
                              bool>::type
      is_valid_(const automaton_t&)
      {
        return true;
      }

      template <star_status_t Status>
      static
      typename std::enable_if<Status == star_status_t::NON_STARRABLE,
                              bool>::type
      is_valid_(const automaton_t& aut)
      {
        return is_proper(aut) || is_eps_acyclic(aut);
      }
    };

    template <typename Aut>
    class is_valider<Aut, false>
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
    public:
      static constexpr bool is_valid(const automaton_t&)
      {
        return true;
      }
    };

  }


  template <typename Aut>
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
      template <typename Aut>
      bool is_valid(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_valid(a);
      }

     REGISTER_DECLARE(is_valid,
                      (const automaton& aut) -> bool);
    }
  }


  /*-------------------.
  | is_valid(ratexp).  |
  `-------------------*/

  template <typename RatExpSet>
  bool
  is_valid(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e)
  {
    rat::constant_term_visitor<RatExpSet> constant_term{rs};
    try
    {
      constant_term(e);
      return true;
    }
    catch (const std::runtime_error&)
    {
      return false;
    }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      bool
      is_valid_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return is_valid<RatExpSet>(e.ratexpset(), e.ratexp());
      }

      REGISTER_DECLARE(is_valid_ratexp, (const ratexp& e) -> bool);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_IS_VALID_HH
