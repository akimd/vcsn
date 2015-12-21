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
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/vector.hh> // make_vector

#include <vcsn/algos/epsilon-remover.hh>
#include <vcsn/algos/epsilon-remover-distance.hh>
#include <vcsn/algos/epsilon-remover-lazy.hh>
#include <vcsn/algos/epsilon-remover-separate.hh>

namespace vcsn
{
  namespace detail
  {
    template <Automaton Aut>
    class properer
    {
      using automaton_t = std::remove_cv_t<Aut>;
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
      properer(automaton_t aut,
               bool prune = true,
               const std::string& algo = "auto")
        : aut_(aut)
        , prune_(prune)
        , algo_(algo)
      {}

      /// Proper automata with proper context
      aut_proper_t operator()() const
      {
        return proper_star<weightset_t::star_status()>();
      }

      /// In-place transition removal
      void here()
      {
        proper_star_here<weightset_t::star_status()>();
      }

    private:

      /*-----------------.
      | Separate proper. |
      `-----------------*/

      aut_proper_t remover_() const
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
        else if (algo_ == "distance")
          {
            detail::epsilon_remover_distance<automaton_t> r(aut_, prune_);
            return r();
          }
        else
          raise("proper: invalid algorithm: ", algo_);
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::TOPS, aut_proper_t>
      proper_star() const
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
      std::enable_if_t<Status == star_status_t::ABSVAL, aut_proper_t>
      proper_star() const
      {
        require(is_valid(aut_), "proper: invalid automaton");
        return remover_();
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::STARRABLE, aut_proper_t>
      proper_star() const
      {
        return remover_();
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::NON_STARRABLE, aut_proper_t>
      proper_star() const
      {
        require(is_valid(aut_), "proper: invalid automaton");
        return remover_();
      }


      /*-----------------.
      | In place proper. |
      `-----------------*/

      void remover_here_()
      {
        if (algo_ == "auto" || algo_ == "default" || algo_ == "inplace")
          {
            detail::epsilon_remover<automaton_t> r(aut_, prune_);
            r.in_situ_remover();
          }
        else if (algo_ == "separate" || algo_ == "distance")
          raise("proper: algorithm ", algo_, " cannot be used in place");
        else
          raise("proper: invalid algorithm: ", algo_);
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::TOPS>
      proper_star_here()
      {
        try
          {
            remover_here_();
          }
        catch (const std::runtime_error&)
          {
            raise("proper: invalid automaton");
          }
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::ABSVAL>
      proper_star_here()
      {
        require(is_valid(aut_), "proper: invalid automaton");
        remover_here_();
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::STARRABLE>
      proper_star_here()
      {
        remover_here_();
      }

      template <star_status_t Status>
      std::enable_if_t<Status == star_status_t::NON_STARRABLE>
      proper_star_here()
      {
        require(is_valid(aut_), "proper: invalid automaton");
        remover_here_();
      }

      automaton_t aut_;
      bool prune_;
      const std::string& algo_;
    };

    template <Automaton Aut>
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
  /// \param algo   how elimination is performed
  ///     "auto"       same as "inplace"
  ///     "default"    same as "inplace"
  ///     "inplace"    eliminate in place
  ///     "separate"   first separate the automaton into spontaneous and
  ///                  proper parts
  ///     "distance"   compute all-pairs distances
  template <Automaton Aut>
  auto
  proper(const Aut& aut, direction dir = direction::backward,
         bool prune = true, const std::string& algo = "auto")
    -> fresh_automaton_t_of<Aut, detail::proper_context<context_t_of<Aut>>>
  {
    switch (dir)
      {
      case direction::backward:
        return detail::make_properer(aut, prune, algo)();
      case direction::forward:
        return transpose(proper(transpose(aut),
                                direction::backward, prune, algo));
      }
    BUILTIN_UNREACHABLE();
  }

  template <Automaton Aut>
  auto
  proper_lazy(const Aut& aut, direction dir = direction::backward,
              bool prune = true)
    -> lazy_proper_automaton<Aut>
  {
    require(dir == direction::backward,
            "backward direction for lazy proper is not implemented");
    return make_shared_ptr<lazy_proper_automaton<Aut>>(aut, prune);
  }

  /// Eliminate spontaneous transitions in place.  Raise if the
  /// automaton was not valid.
  ///
  /// \param aut   the input automaton
  /// \param dir   whether backward or forward elimination
  /// \param prune whether to suppress states becoming inaccessible
  template <Automaton Aut>
  inline
  void proper_here(Aut& aut, direction dir = direction::backward,
                   bool prune = true, const std::string& algo = "auto")
  {
    switch (dir)
      {
      case direction::backward:
        detail::make_properer(aut, prune, algo).here();
        return;
      case direction::forward:
        auto tr_aut = transpose(aut);
        detail::make_properer(tr_aut, prune, algo).here();
        return;
      }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Dir, typename Bool, typename String>
      automaton proper(const automaton& aut, direction dir, bool prune,
                       const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "lazy")
          return make_automaton(proper_lazy(a, dir, prune));
        else
          return make_automaton(::vcsn::proper(a, dir, prune, algo));
      }
    }
  }
} // namespace vcsn
