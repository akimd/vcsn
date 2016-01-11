#pragma once

#include <set>
#include <stack>
#include <string>
#include <type_traits>
#include <queue>

#include <vcsn/algos/tags.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/dynamic_bitset.hh>
#include <vcsn/misc/map.hh> // vcsn::has
#include <vcsn/misc/raise.hh> // b
#include <vcsn/misc/unordered_map.hh> // vcsn::has
#include <vcsn/weightset/fwd.hh> // b
#include <vcsn/weightset/polynomialset.hh>
#include <vcsn/labelset/stateset.hh>

namespace vcsn
{
  /// Request the Boolean specialization for determinization (B and
  /// F2).
  struct boolean_tag {};

  namespace detail
  {
    /// The best tag depending on the type of Aut.
    template <Automaton Aut>
    using determinization_tag
      = std::conditional_t<std::is_same<weightset_t_of<Aut>, b>::value,
                           boolean_tag,
                           weighted_tag>;
  }

  /*----------------------.
  | subset construction.  |
  `----------------------*/

  namespace detail
  {
    /// \brief The subset construction automaton from another.
    ///
    /// \tparam Aut the input automaton type.
    ///
    /// \pre labelset is free.
    /// \pre weightset is Boolean.
    template <Automaton Aut>
    class determinized_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<Aut, Ctx>;
      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      /// The name: set of (input) states.
      using state_name_t = dynamic_bitset;

      /// Result automaton state type.
      using state_t = state_t_of<automaton_t>;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      determinized_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
        , finals_(state_size_)
      {
        // Pre.
        state_name_t n;
        n.resize(state_size_);
        n.set(input_->pre());
        todo_.push(map_.emplace(n, super_t::pre()).first);

        // Final states.
        for (auto t : final_transitions(input_))
          finals_.set(input_->src_of(t));
      }

      static symbol sname()
      {
        static auto res = symbol{"determinized_automaton<"
                                 + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "determinized_automaton<";
        input_->print_set(o, fmt);
        return o << '>';
      }

      /// Determinize all accessible states.
      void operator()()
      {
        using dests_t
          = std::map<label_t, state_name_t, vcsn::less<labelset_t>>;
        auto dests = dests_t{};
        while (!todo_.empty())
          {
            state_t src = todo_.top()->second;
            const auto& ss = todo_.top()->first;
            todo_.pop();

            dests.clear();
            for (auto s = ss.find_first(); s != ss.npos;
                 s = ss.find_next(s))
              {
                // Cache the output transitions of state s.
                auto i = successors_.find(s);
                if (i == successors_.end())
                  {
                    i = successors_.emplace(s, label_map_t{}).first;
                    auto& j = i->second;
                    for (auto t : out(input_, s))
                      {
                        auto l = input_->label_of(t);
                        if (j.find(l) == j.end())
                          j[l].resize(state_size_);
                        j[l].set(input_->dst_of(t));
                      }
                  }

                // Store in dests the possible destinations per label.
                for (const auto& p : i->second)
                  {
                    auto j = dests.find(p.first);
                    if (j == dests.end())
                      dests[p.first] = p.second;
                    else
                      j->second |= p.second;
                  }
              }

            // Outgoing transitions from the current (result) state.
            for (const auto& d : dests)
              this->new_transition(src, state_(d.second), d.first);
          }
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool delimit = false) const
      {
        auto i = origins().find(s);
        if (i == std::end(origins()))
          this->print_state(s, o);
        else
          {
            if (delimit)
              o << '{';
            const char* sep = "";
            for (auto s: i->second)
              {
                o << sep;
                input_->print_state_name(s, o, fmt, true);
                sep = ", ";
              }
            if (delimit)
              o << '}';
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      mutable origins_t origins_;

      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            {
              auto from = std::set<state_t>{};
              const auto& ss = p.first;
              for (auto s = ss.find_first(); s != ss.npos;
                   s = ss.find_next(s))
                from.emplace(s);
              origins_.emplace(p.second, std::move(from));
            }
        return origins_;
      }

    private:
      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state_(const state_name_t& ss)
      {
        state_t res;
        auto i = map_.find(ss);
        if (i == std::end(map_))
          {
            res = this->new_state();
            todo_.push(map_.emplace(ss, res).first);

            if (ss.intersects(finals_))
              this->set_final(res);
          }
        else
          res = i->second;
        return res;
      }

      /// Set of input states -> output state.
      using map_t = std::unordered_map<state_name_t, state_t>;
      map_t map_;

      /// Input automaton.
      automaton_t input_;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;

      /// The sets of (input) states waiting to be processed.
      using stack = std::stack<typename map_t::const_iterator>;
      stack todo_;

      /// Set of final states in the input automaton.
      state_name_t finals_;

      /// successors[SOURCE-STATE][LABEL] = DEST-STATESET.
      using label_map_t = std::unordered_map<label_t, state_name_t,
                                             vcsn::hash<labelset_t>,
                                             vcsn::equal_to<labelset_t>>;
      using successors_t = std::map<state_t, label_map_t>;
      successors_t successors_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <Automaton Aut>
  using determinized_automaton
    = std::shared_ptr<detail::determinized_automaton_impl<Aut>>;

  template <Automaton Aut>
  auto
  determinize(const Aut& a, boolean_tag)
    -> determinized_automaton<Aut>
  {
    // Asserting here instead of inside the implementation to
    // avoid static_assert to be raised in this kind of cases:
    // auto func(const Aut& a)
    //   -> std::enable_if_t<false_condition, decltype(determinize(a))>
    // We can then declare our function this way:
    // auto func(const Aut& a)
    //   -> std::enable_if_t<false_condition, determinized_automaton<Aut>>
    static_assert(labelset_t_of<Aut>::is_free(),
                  "determinize: requires free labelset");
    static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                  "determinize: boolean: requires Boolean weights");

    auto res = make_shared_ptr<determinized_automaton<Aut>>(a);
    // Determinize.
    res->operator()();
    return res;
  }


  /*---------------------------.
  | weighted determinization.  |
  `---------------------------*/
  namespace detail
  {
    /// \brief The weighted determinization of weighted automaton.
    ///
    /// \tparam Aut the input weighted automaton type.
    ///
    /// \pre labelset is free.
    template <Automaton Aut>
    class detweighted_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: requires free labelset");

    public:
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<Aut, Ctx>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      using stateset_t = stateset<automaton_t>;
      using state_nameset_t = polynomialset<context<stateset_t, weightset_t>>;
      using state_name_t = typename state_nameset_t::value_t;

      /// Build the weighted determinizer.
      /// \param a         the weighted automaton to determinize
      detweighted_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
      {
        // Pre.
        state_name_t n;
        n.set(input_->pre(), ws_.one());
        todo_.push(map_.emplace(n, super_t::pre()).first);

        // Post.
        n.clear();
        n.set(input_->post(), ws_.one());
        map_[n] = super_t::post();
      }

      static symbol sname()
      {
        static auto res = symbol{"detweighted_automaton<"
                                 + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "detweighted_automaton<";
        input_->print_set(o, fmt);
        return o << '>';
      }

      /// The determinization of weighted automaton.
      void operator()()
      {
        // label -> <destination, sum of weights>.
        using dests_t
          = std::map<label_t, state_name_t, vcsn::less<labelset_t>>;
        auto dests = dests_t{};
        while (!todo_.empty())
          {
            state_t src = todo_.front()->second;
            const auto& ss = todo_.front()->first;
            todo_.pop();

            dests.clear();
            for (const auto& p : ss)
              {
                auto s = label_of(p);
                auto v = weight_of(p);
                for (auto t : all_out(input_, s))
                  {
                    auto l = input_->label_of(t);
                    auto dst = input_->dst_of(t);
                    auto w = ws_.mul(v, input_->weight_of(t));

                    // For each letter, update destination state, and
                    // sum of weights.
                    if (!has(dests, l))
                      dests.emplace(l, ns_.zero());
                    auto& d = dests[l];
                    ns_.add_here(d, dst, w);
                  }
              }

            for (auto& d : dests)
              if (!ns_.is_zero(d.second))
                {
                  weight_t w = ns_.normalize_here(d.second);
                  this->new_transition(src, state_(d.second),
                                       d.first, w);
                }
          }
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t ss, std::ostream& o,
                       format fmt = {}, bool delimit = false) const
      {
        auto i = origins().find(ss);
        if (i == origins().end())
          this->print_state(ss, o);
        else
          {
            if (delimit)
              o << '{';
            ns_.print(i->second, o, fmt, ", ");
            if (delimit)
              o << '}';
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }

    private:
      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state_(const state_name_t& name)
      {
        state_t res;
        auto i = map_.find(name);
        if (i == std::end(map_))
          {
            res = this->new_state();
            todo_.push(map_.emplace(name, res).first);
          }
        else
          res = i->second;
        return res;
      };

      /// Map from state name to state number.
      using map_t = std::unordered_map<state_name_t, state_t,
                                       vcsn::hash<state_nameset_t>,
                                       vcsn::equal_to<state_nameset_t>>;
      map_t map_;

      /// Input automaton.
      automaton_t input_;

      /// Its weightset.
      weightset_t ws_ = *input_->weightset();

      /// (Nameset) The polynomialset that stores weighted states.
      state_nameset_t ns_ = {{stateset_t(input_), ws_}};

      /// The sets of (input) states waiting to be processed.
      using queue = std::queue<typename map_t::const_iterator>;
      queue todo_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <Automaton Aut>
  using detweighted_automaton
    = std::shared_ptr<detail::detweighted_automaton_impl<Aut>>;

  /// Determinization: weighted, general case.
  template <Automaton Aut>
  auto
  determinize(const Aut& a, weighted_tag)
    -> detweighted_automaton<Aut>
  {
    auto res = make_shared_ptr<detweighted_automaton<Aut>>(a);
    res->operator()();
    return res;
  }


  /// Determinization: automatic dispatch based on the automaton type.
  template <Automaton Aut>
  auto
  determinize(const Aut& a, auto_tag = {})
  {
    return determinize(a, detail::determinization_tag<Aut>{});
  }



  /*-------------------.
  | dyn::determinize.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Enable if Aut is Boolean.
      template <Automaton Aut, typename Type = void>
      using enable_if_boolean_t
        = std::enable_if_t<std::is_same<weightset_t_of<Aut>, b>::value, Type>;

      /// Enable if Aut is not Boolean.
      template <Automaton Aut, typename Type = void>
      using enable_if_not_boolean_t
        = std::enable_if_t<!std::is_same<weightset_t_of<Aut>, b>::value, Type>;


      /// Boolean Bridge.
      template <Automaton Aut, typename String>
      enable_if_boolean_t<Aut, automaton>
      determinize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "auto" || algo == "boolean")
          return make_automaton(::vcsn::determinize(a, boolean_tag{}));
        else if (algo == "weighted")
          return make_automaton(::vcsn::determinize(a, weighted_tag{}));
        else
          raise("determinize: invalid algorithm: ", str_escape(algo));
      }

      /// Weighted Bridge.
      template <Automaton Aut, typename String>
      enable_if_not_boolean_t<Aut, automaton>
      determinize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "boolean")
          raise("determinize: cannot apply Boolean determinization");
        else if (algo == "auto" || algo == "weighted")
          return make_automaton(::vcsn::determinize(a, weighted_tag{}));
        else
          raise("determinize: invalid algorithm: ", str_escape(algo));
      }

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      determinize(const automaton& aut, const std::string& algo)
      {
        return determinize_<Aut, String>(aut, algo);
      }
    }
  }


  /*----------------.
  | codeterminize.  |
  `----------------*/

  template <Automaton Aut, typename Tag = auto_tag>
  auto
  codeterminize(const Aut& aut, Tag tag = {})
  {
    return transpose(determinize(transpose(aut), tag));
  }

  /*---------------------.
  | dyn::codeterminize.  |
  `---------------------*/

  // FIXME: duplicate code with determinize.
  namespace dyn
  {
    namespace detail
    {
      /// Boolean Bridge.
      template <Automaton Aut, typename String>
      enable_if_boolean_t<Aut, automaton>
      codeterminize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "auto" || algo == "boolean")
          return make_automaton(::vcsn::codeterminize(a, boolean_tag{}));
        else if (algo == "weighted")
          return make_automaton(::vcsn::codeterminize(a, weighted_tag{}));
        else
          raise("codeterminize: invalid algorithm: ", str_escape(algo));
      }

      /// Weighted Bridge.
      template <Automaton Aut, typename String>
      enable_if_not_boolean_t<Aut, automaton>
      codeterminize_(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "boolean")
          raise("codeterminize: cannot apply Boolean determinization");
        else if (algo == "auto" || algo == "weighted")
          return make_automaton(::vcsn::codeterminize(a));
        else
          raise("codeterminize: invalid algorithm: ", str_escape(algo));
      }

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      codeterminize(const automaton& aut, const std::string& algo)
      {
        return codeterminize_<Aut, String>(aut, algo);
      }
    }
  }
} // namespace vcsn
