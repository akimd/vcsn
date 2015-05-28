#pragma once

#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/dot.hh>
#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/labelset/labelset.hh> // make_proper_context
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/vector.hh> // make_vector

#include <vcsn/algos/epsilon-remover-separate.hh>
#include <vcsn/algos/epsilon-remover.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename Aut>
    class properer
    {
      using automaton_t = typename std::remove_cv<Aut>::type;
      using weightset_t = weightset_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using aut_proper_t = fresh_automaton_t_of<automaton_t,
                            detail::proper_context<context_t_of<automaton_t>>>;

    public:
      /**@brief Remove the epsilon-transitions of the input

        The behaviour of this method depends on the star_status of the
        weight_set:

         -- starrable : always valid, does not throw any exception
         -- tops : the proper algo is directly launched on the input;
         if it returns false, an exception is launched
         -- non_starrable / absval:
         is_valid is called before launching the algorithm.
         @param aut The automaton in which epsilon-transitions will be removed
         @param prune   Whether to remove states that become inaccessible.
         @throw runtime_error if the input is not valid
      */
      properer(const automaton_t& aut,
               bool prune = true,
               const std::string& algo = "auto")
        : aut_(aut)
        , prune_(prune)
        , algo_(algo)
      {}

      aut_proper_t operator()()
      {
        return proper_star<weightset_t::star_status()>();
      }

    private:
      aut_proper_t remover_()
      {
        if (algo_ == "auto" || algo_ == "default" || algo_ == "inplace")
          {
            auto a = copy(aut_); // in place
            detail::epsilon_remover<decltype(a)> r(a, prune_);
            return r();
          }
        else if (algo_ == "separate")
          {
            detail::epsilon_remover_separate<automaton_t> r(aut_, prune_);
            return r();
          }
        else
          raise("proper: invalid algorithm: ", algo_);
      }

      template <star_status_t Status>
      vcsn::enable_if_t<Status == star_status_t::TOPS, aut_proper_t>
      proper_star()
      {
        try
          {
            return remover_();
          }
        catch (const std::runtime_error&)
          {
            raise("proper: invalid automaton");
          }
      }

      template <star_status_t Status>
      vcsn::enable_if_t<Status == star_status_t::ABSVAL, aut_proper_t>
      proper_star()
      {
        require(is_valid(aut_), "proper: invalid automaton");
        return remover_();
      }

      template <star_status_t Status>
      vcsn::enable_if_t<Status == star_status_t::STARRABLE, aut_proper_t>
      proper_star()
      {
        return remover_();
      }

      template <star_status_t Status>
      vcsn::enable_if_t<Status == star_status_t::NON_STARRABLE, aut_proper_t>
      proper_star()
      {
        require(is_valid(aut_), "proper: invalid automaton");
        return remover_();
      }

      automaton_t aut_;
      bool prune_;
      const std::string& algo_;
    };

    template <typename Aut>
    auto make_properer(Aut aut,
                       bool prune = true,
                       const std::string& algo = "auto")
    {
      return properer<Aut>(aut, prune, algo);
    }
  }

  /*---------.
  | proper.  |
  `---------*/

  /// Eliminate spontaneous transitions.  Raise if the input automaton
  /// is invalid.
  ///
  /// \param aut   the input automaton
  /// \param dir   whether backward or forward elimination
  /// \param prune whether to suppress states becoming inaccessible
  template <typename Aut>
  auto
  proper(const Aut& aut, direction dir = direction::backward,
         bool prune = true, const std::string& algo = "auto")
    -> fresh_automaton_t_of<Aut,
                            detail::proper_context<context_t_of<Aut>>>
  {
    switch (dir)
      {
      case direction::backward:
        {
          auto p = detail::make_properer(aut, prune, algo);
          return p();
        }
      case direction::forward:
        {
          auto p = detail::make_properer(transpose(aut), prune, algo);
          return transpose(p());
        }
      }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Dir, typename Bool, typename String>
      automaton proper(const automaton& aut, direction dir, bool prune,
                       const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::proper(a, dir, prune, algo));
      }
    }

  }

} // namespace vcsn
