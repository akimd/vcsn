#pragma once

#include <deque>
#include <iostream>
#include <map>
#include <utility>

#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/state-bimap.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/tuple.hh>

namespace vcsn
{
  namespace detail
  {

    /// A valueset to manipulate tuples of states.
    ///
    /// \tparam Ranked  whether to keep a rank per state
    /// \tparam Auts    the types of the input automata
    template <bool Ranked, Automaton... Auts>
    struct state_tupleset
    {
      /// State names: Tuple of states of input automata.
      using state_name_t
        = std::conditional_t<Ranked,
                             std::tuple<state_t_of<Auts>..., unsigned short>,
                             std::tuple<state_t_of<Auts>...>>;

      /// State names are the values we manipulate.
      using value_t = state_name_t;

      /// Hash a state name.
      static auto hash(const value_t& v)
      {
        return vcsn::detail::hash_value(v);
      }

      /// Whether are equal.
      static auto equal(const value_t& v1, const value_t& v2)
      {
        return v1 == v2;
      }
    };

    /*----------------------------------------------.
    | tuple_automaton_impl<Ranked, Aut, Auts...>.   |
    `----------------------------------------------*/

    /// An automaton whose states are tuples of states of automata.
    ///
    /// Corresponds to the Cartesian product of states.
    ///
    /// \tparam Ranked  whether to keep a rank per state
    /// \tparam Lazy    whether to maintain origins incrementally.
    /// \tparam Aut     the output automaton type
    /// \tparam Auts    the types of the input automata
    template <bool Ranked, bool Lazy, Automaton Aut, Automaton... Auts>
    class tuple_automaton_impl
      : public automaton_decorator<Aut>
      , public state_bimap<state_tupleset<Ranked, Auts...>,
                           stateset<Aut>,
                           true>
    {
    public:
      /// The type of the resulting automaton.
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;

      using state_bimap_t
        = state_bimap<state_tupleset<Ranked, Auts...>,
                      stateset<Aut>,
                      true>;

      /// Result state type.
      using state_t = typename super_t::state_t;
      /// State names: Tuple of states of input automata.
      using state_name_t = typename state_bimap_t::state_name_t;

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
        this->emplace(pre_(), aut_->pre());
        this->emplace(post_(), aut_->post());
      }

      bool state_has_name(state_t s) const
      {
        return has(this->origins(), s);
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

      // FIXME: protected:
      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      /// The list of automaton indices as a static list.
      constexpr static size_t rank = sizeof...(Auts);
      using indices_t = vcsn::detail::make_index_sequence<sizeof...(Auts)>;
      static constexpr indices_t indices{};

      /// The sname of the sub automata.
      template <typename... T>
      static std::string sname_(const T&... t)
      {
        // tuple_automaton<Ranked, SupportAutomaton, InputAutomaton...>.
        auto res = std::string{"<"};
        res += (Ranked ? "true" : "false");
        res += ", ";
        res += (Lazy ? "true" : "false");
        using swallow = int[];
        (void) swallow
          {
            (res += ", " + t,
             std::cerr << "Wow! " << t << '\n',
             0)...
          };
        res += ", " + Aut::element_type::sname();
        (void) swallow
          {
            (res += ", " + Auts::element_type::sname(),
             0)...
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
        o << '<' << (Ranked ? "true" : "false") << ", ";
        o << '<' << (Lazy ? "true" : "false") << ", ";
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
        using std::get;
        return static_if<Ranked>
          ([this](auto... ss) { return state_name_t{ss..., 0}; },
           [this](auto... ss) { return state_name_t{ss...}; })
          (get<I>(auts_)->pre()...);
      }

      /// The name of the post of the output automaton.
      state_name_t post_() const
      {
        return post_(indices);
      }

      template <size_t... I>
      state_name_t post_(seq<I...>) const
      {
        using std::get;
        return static_if<Ranked>
          ([this](auto... ss) { return state_name_t{ss..., 0}; },
           [this](auto... ss) { return state_name_t{ss...}; })
          (get<I>(auts_)->post()...);
      }

      /// The state corresponding to a tuple of states of operands.
      ///
      /// If this is a new state, add to the worklist and update the
      /// map.
      state_t state(const state_name_t& sn)
      {
        state_t res;
        auto i = this->find_key(sn);
        if (i == this->end_key())
          {
            res = this->new_state();
            if (Lazy)
              this->set_lazy(res);
            // FIXME: todo_.push(this->emplace(std::move(sn), res).first);
            this->emplace(sn, res);
            todo_.emplace_back(sn, res);
          }
        else
          res = i->second;
        return res;
      }

      template <bool Ranked_ = Ranked>
      auto state(state_t_of<Auts>... ss, unsigned short rank)
        -> std::enable_if_t<Ranked_, state_t>
      {
        return state(std::make_tuple(ss..., rank));
      }

      template <bool Ranked_ = Ranked>
      auto state(state_t_of<Auts>... ss)
        -> std::enable_if_t<!Ranked_, state_t>
      {
        return state(std::make_tuple(ss...));
      }

      template <size_t... I>
      std::ostream&
      print_state_name_(state_t s, std::ostream& o, format fmt,
                        bool delimit, seq<I...> indices) const
      {
        const auto& origs = this->origins();
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
        static_if<Ranked>([&o](const auto& sn){
            o << ", " << std::get<rank>(sn);
          })(sn);
        if (delimit)
          o << ')';
        return o;
      }

      /// Input automata, supplied at construction time.
      automata_t auts_;

      /// Worklist of state tuples.
      std::deque<std::pair<state_name_t, state_t>> todo_;
    };
  }

  /// A tuple automaton as a shared pointer.
  template <bool Ranked, bool Lazy, Automaton... Auts>
  using tuple_automaton
    = std::shared_ptr<detail::tuple_automaton_impl<Ranked, Lazy, Auts...>>;

  template <bool Ranked, bool Lazy, Automaton... Auts>
  auto
  make_tuple_automaton(const Auts&... auts)
    -> tuple_automaton<Ranked, Lazy, Auts...>
  {
    using res_t = tuple_automaton<Ranked, Lazy, Auts...>;
    return make_shared_ptr<res_t>(auts...);
  }
}
