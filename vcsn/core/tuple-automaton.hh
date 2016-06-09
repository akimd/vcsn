#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <utility>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/bimap.hh> // has
#include <vcsn/misc/map.hh> // has
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/tuple.hh>

namespace vcsn
{
  namespace detail
  {
    /*-------------------------------------.
    | tuple_automaton_impl<Aut, Auts...>.  |
    `-------------------------------------*/

    /// An automaton whose states are tuples of states of automata.
    ///
    /// Corresponds to the Cartesian product of states.
    ///
    /// \tparam Aut   the output automaton type
    /// \tparam Auts  the input automaton types
    template <Automaton Aut, Automaton... Auts>
    class tuple_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;

      /// Result state type.
      using state_t = typename super_t::state_t;
      /// State names: Tuple of states of input automata.
      using state_name_t = std::tuple<state_t_of<Auts>...>;

      /// The type of context of the result.
      ///
      /// The type is the "join" of the contexts, independently of the
      /// algorithm.  However, its _value_ differs: in the case of the
      /// product, the labelset is the meet of the labelsets, it is
      /// its join for shuffle and infiltrate.
      using context_t = context_t_of<Aut>;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;

      using label_t = typename labelset_t::value_t;
      using weight_t = typename weightset_t::value_t;

      static symbol sname()
      {
        static auto res = symbol{"tuple_automaton" + sname_()};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "tuple_automaton";
        print_set_(o, fmt);
        return o;
      }

      /// The type of input automata.
      using automata_t = std::tuple<Auts...>;

      /// The type of the Ith input automaton, unqualified.
      template <size_t I>
      using input_automaton_t = base_t<tuple_element_t<I, automata_t>>;

      /// The result automaton.
      using super_t::aut_;

      tuple_automaton_impl(const automaton_t& aut, const Auts&... auts)
        : super_t(aut)
        , auts_(auts...)
      {
        pmap_().insert({pre_(), aut_->pre()});
        pmap_().insert({post_(), aut_->post()});
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      /// Print a state name from its state index.
      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {}, bool delimit = false) const
      {
        return print_state_name_(s, o, fmt, delimit, indices);
      }

      /// Print a state name from its state name.
      std::ostream&
      print_state_name(const state_name_t& sn, std::ostream& o,
                       format fmt = {}, bool delimit = false) const
      {
        return print_state_name_(sn, o, fmt, delimit, indices);
      }

      using bimap_t
        = boost::bimap<boost::bimaps::unordered_set_of<state_name_t>,
                       boost::bimaps::unordered_set_of<state_t>>;
      using map_t = typename bimap_t::left_map;
      using origins_t = typename bimap_t::right_map;


      /// A map from result state to tuple of original states.
      const origins_t&
      origins() const
      {
        return bimap_.right;
      }

      // FIXME: protected:
      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      /// The list of automaton indices as a static list.
      using indices_t = vcsn::detail::make_index_sequence<sizeof...(Auts)>;
      static constexpr indices_t indices{};

      /// The sname of the sub automata.
      template <typename... T>
      static std::string sname_(const T&... t)
      {
        std::string res = "<" ;
        using swallow = int[];
        const char* sep = "";
        (void) swallow
          {
            (res += sep + t, sep = ", ", 0)...
          };
        res += sep + Aut::element_type::sname();
        (void) swallow
          {
            (res += ", " + Auts::element_type::sname(), 0)...
          };
        res += ">";
        return res;
      }

      /// The setname of the sub automata.
      std::ostream& print_set_(std::ostream& o, format fmt) const
      {
        return print_set_(o, fmt, indices);
      }

      template <size_t... I>
      std::ostream& print_set_(std::ostream& o, format fmt,
                               seq<I...>) const
      {
        o << '<';
        aut_->print_set(o, fmt);
        o << ", ";
        const char* sep = "";
        using swallow = int[];
        (void) swallow
          {
            (o << sep,
             std::get<I>(auts_)->print_set(o, fmt),
             sep = ", ",
             0)...
          };
        return o << '>';
      }

      /// The name of the pre of the output automaton.
      state_name_t pre_() const
      {
        return pre_(indices);
      }

      template <size_t... I>
      state_name_t pre_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called.
        return state_name_t{(std::get<I>(auts_)->pre())...};
      }

      /// The name of the post of the output automaton.
      state_name_t post_() const
      {
        return post_(indices);
      }

      template <size_t... I>
      state_name_t post_(seq<I...>) const
      {
        // clang 3.4 on top of libstdc++ wants this ctor to be
        // explicitly called.
        return state_name_t{(std::get<I>(auts_)->post())...};
      }

      /// The state in the product corresponding to a pair of states
      /// of operands.
      ///
      /// Add the given two source-automaton states to the worklist
      /// for the given result automaton if they aren't already there,
      /// updating the map; in any case return.
      template <bool Lazy = false>
      state_t state(const state_name_t& state)
      {
        auto lb = pmap_().find(state);
        if (lb == pmap_().end())
          {
            state_t s = aut_->new_state();
            if (Lazy)
              aut_->set_lazy(s, true);
            lb = pmap_().insert(lb, {state, s});
            todo_.emplace_back(state, s);
          }
        return lb->second;
      }

      template <bool Lazy = false>
      state_t state(state_t_of<Auts>... ss)
      {
        return state<Lazy>(std::make_tuple(ss...));
      }

      template <size_t... I>
      std::ostream&
      print_state_name_(state_t s, std::ostream& o, format fmt,
                        bool delimit, seq<I...> indices) const
      {
        const auto& origs = origins();
        auto i = origs.find(s);
        if (i == std::end(origs))
          this->print_state(s, o);
        else
          print_state_name_(i->second, o, fmt, delimit, indices);
        return o;
      }

      template <size_t... I>
      std::ostream&
      print_state_name_(const state_name_t& sn, std::ostream& o,
                        format fmt, bool delimit, seq<I...>) const
      {
        if (delimit)
          o << '(';
        const char* sep = "";
        using swallow = int[];
        (void) swallow
          {
            (o << sep,
             std::get<I>(auts_)->print_state_name(std::get<I>(sn),
                                                  o, fmt, true),
             sep = ", ",
             0)...
          };
        if (delimit)
          o << ')';
        return o;
      }

      /// A map from original state and status (spontaneous or proper state)
      /// to result state.
      map_t&
      pmap_()
      {
        return bimap_.left;
      }

      /// Input automata, supplied at construction time.
      automata_t auts_;

      /// Bijective map state_name_t -> state_t
      mutable bimap_t bimap_;

      /// Worklist of state tuples.
      std::deque<std::pair<state_name_t, state_t>> todo_;
    };
  }

  /// A tuple automaton as a shared pointer.
  template <Automaton... Auts>
  using tuple_automaton
    = std::shared_ptr<detail::tuple_automaton_impl<Auts...>>;

  template <Automaton... Auts>
  auto
  make_tuple_automaton(const Auts&... auts)
    -> tuple_automaton<Auts...>
  {
    using res_t = tuple_automaton<Auts...>;
    return make_shared_ptr<res_t>(auts...);
  }
}
