#pragma once

#include <string>

#include <vcsn/algos/tags.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton.hh>
#include <vcsn/core/automaton-decorator.hh> // all_out
#include <vcsn/core/polystate-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*-------------------------------.
  | Function tags and properties.  |
  `-------------------------------*/

  CREATE_FUNCTION_TAG(codeterminize);
  CREATE_FUNCTION_TAG(determinize);

  template <>
  struct function_prop<determinize_ftag>
  {
    // New automaton, no need to invalidate.
    static const bool invalidate = false;

    static auto& updated_prop()
    {
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      static auto updated_prop = create_updated_prop(
        {
          { is_deterministic_ptag::id(), boost::any{true} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

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
    template <Automaton Aut, wet_kind_t Kind, bool Lazy = false>
    class determinized_automaton_impl
      : public automaton_decorator<polystate_automaton<Aut, Kind, Lazy>>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: requires free labelset");

    public:
      using automaton_t = Aut;
      constexpr static wet_kind_t kind = Kind;
      using super_t
        = automaton_decorator<polystate_automaton<automaton_t, kind, Lazy>>;
      using self_t = determinized_automaton_impl;

      /// Labels and weights.
      using context_t = context_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      /// State index.
      using stateset_t = stateset<automaton_t>;
      using state_t = state_t_of<automaton_t>;

      /// The state name: set of (input) states.
      using state_name_t = typename super_t::element_type::state_name_t;

      using super_t::aut_;
      auto strip() const
      {
        return aut_->strip();
      }
      auto origins() const
      {
        return aut_->origins();
      }

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      determinized_automaton_impl(const automaton_t& a)
        : super_t{make_polystate_automaton<automaton_t, kind, Lazy>(a)}
      {
        // Final states.
        for (auto t : final_transitions(aut_->input_))
          aut_->ns_.new_weight(finals_,
                               aut_->input_->src_of(t),
                               aut_->input_->weight_of(t));
      }

      static symbol sname()
      {
        static auto res = symbol{"determinized_automaton<"
                                 + automaton_t::element_type::sname()
                                 + ", " + to_string(kind)
                                 + ", " + (Lazy ? "true" : "false")
                                 + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "determinized_automaton<";
        aut_->input_->print_set(o, fmt);
        return o << ", " << to_string(kind)
                 << ", " << Lazy
                 << '>';
      }

      /// Determinize the automaton.
      void operator()()
      {
        while (!aut_->todo_.empty())
          {
            state_t src = aut_->state(aut_->todo_.front());
            const auto& ss = aut_->state_name(aut_->todo_.front());
            aut_->todo_.pop();
            complete_(src, ss);
          }
        aut_->properties().update(determinize_ftag{});
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
        -> decltype(all_out(aut_, s))
      {
        if (Lazy && aut_->is_lazy(s))
          complete_(s);
        return aut_->all_out(s);
      }

    private:
      /// Complete a state: find its outgoing transitions.
      void complete_(state_t s) const
      {
        const auto& orig = origins();
        const_cast<self_t&>(*this).complete_(s, orig.at(s));
      }

      /// Compute the outgoing transitions of this state.
      /// \pre weightset is B or F2.
      template <wet_kind_t K = kind>
      auto complete_(state_t src, const state_name_t& ss)
        -> std::enable_if_t<K == wet_kind_t::bitset>
      {
        static_assert(std::is_same<weight_t_of<Aut>, bool>::value,
                      "determinize: boolean: requires B or F2 weights");
        if (Lazy)
          aut_->set_lazy(src, false);
        // label -> <destination, sum of weights>.
        using dests_t
          = std::map<label_t, state_name_t, vcsn::less<labelset_t>>;
        auto dests = dests_t{};
        for (const auto& p : ss)
          {
            auto s = label_of(p);
            // Cache the output transitions of state s.
            auto i = successors_.find(s);
            if (i == successors_.end())
              {
                i = successors_.emplace(s, label_map_t{}).first;
                auto& j = i->second;
                for (auto t : out(aut_->input_, s))
                  {
                    auto l = aut_->input_->label_of(t);
                    auto dst = aut_->input_->dst_of(t);
                    if (j.find(l) == j.end())
                      j.emplace(l, aut_->zero());
                    aut_->ns_.new_weight(j[l], dst, aut_->ws_.one());
                  }
              }

            // Store in dests the possible destinations per label.
            for (const auto& p : i->second)
              {
                auto j = dests.find(p.first);
                if (j == dests.end())
                  dests[p.first] = p.second;
                else
                  aut_->ns_.add_here(j->second, p.second);
              }
          }

        // Outgoing transitions from the current (result) state.
        for (auto& d : dests)
          // Don't create transitions to the empty state.
          if (!aut_->ns_.is_zero(d.second))
            this->new_transition(src, aut_->state_(std::move(d.second)),
                                 d.first);

        auto w = aut_->ns_.scalar_product(ss, finals_);
        if (!aut_->ws_.is_zero(w))
          this->set_final(src, w);
      }

      /// Compute the outgoing transitions of this state.
      template <wet_kind_t K = kind>
      auto complete_(state_t src, const state_name_t& ss)
        -> std::enable_if_t<K != wet_kind_t::bitset>
      {
        if (Lazy)
          aut_->set_lazy(src, false);
        // label -> <destination, sum of weights>.
        using dests_t
          = std::map<label_t, state_name_t, vcsn::less<labelset_t>>;
        auto dests = dests_t{};
        for (const auto& p : ss)
          {
            auto s = label_of(p);
            auto v = weight_of(p);
            for (auto t : out(aut_->input_, s))
              {
                auto l = aut_->input_->label_of(t);
                auto dst = aut_->input_->dst_of(t);
                auto w = aut_->ws_.mul(v, aut_->input_->weight_of(t));

                // For each letter, update destination state, and
                // sum of weights.
                if (!has(dests, l))
                  dests.emplace(l, aut_->zero());
                aut_->ns_.add_here(dests[l], dst, w);
              }
          }

        // Outgoing transitions from the current (result) state.
        for (auto& d : dests)
          // Don't create transitions to the empty state.
          if (!aut_->ns_.is_zero(d.second))
            {
              weight_t w = aut_->ns_.normalize_here(d.second);
              this->new_transition(src, aut_->state_(std::move(d.second)),
                                   d.first, w);
            }

        auto w = aut_->ns_.scalar_product(ss, finals_);
        if (!aut_->ws_.is_zero(w))
          this->set_final(src, w);
      }

      /// Set of final states in the input automaton.
      state_name_t finals_ = aut_->zero();

      /// successors[SOURCE-STATE][LABEL] = DEST-STATESET.
      using label_map_t = std::unordered_map<label_t, state_name_t,
                                             vcsn::hash<labelset_t>,
                                             vcsn::equal_to<labelset_t>>;
      using successors_t = std::map<state_t, label_map_t>;
      successors_t successors_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <Automaton Aut, wet_kind_t Kind, bool Lazy = false>
  using determinized_automaton
  = std::shared_ptr<detail::determinized_automaton_impl<Aut, Kind, Lazy>>;

  template <Automaton Aut, typename Tag, bool Lazy = false>
  auto
  determinize(const Aut& a, Tag = {}, bool_constant<Lazy> = {})
  {
    constexpr auto kind =
      std::is_same<Tag, boolean_tag>::value
      ? wet_kind_t::bitset
      : detail::wet_kind<labelset_t_of<Aut>, weightset_t_of<Aut>>();
    auto res = make_shared_ptr<determinized_automaton<Aut, kind, Lazy>>(a);
    // Determinize.
    if (!Lazy)
      res->operator()();
    return res;
  }

  namespace detail
  {
    /// The best tag depending on the type of Aut.
    template <Automaton Aut>
    using determinization_tag
      = std::conditional_t<std::is_same<weight_t_of<Aut>, bool>::value,
                           boolean_tag,
                           weighted_tag>;
  }

  /// Determinization: automatic dispatch based on the automaton type.
  template <Automaton Aut, bool Lazy = false>
  auto
  determinize(const Aut& a, auto_tag = {}, bool_constant<Lazy> lazy = {})
  {
    try
      {
        return determinize(a, detail::determinization_tag<Aut>{}, lazy);
      }
    catch(const std::runtime_error& e)
      {
        raise(e, "  while determinizing");
      }
  }



  /*-------------------.
  | dyn::determinize.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Enable if Aut is over Booleans.
      template <Automaton Aut, typename Type = void>
      using enable_if_boolean_t
        = std::enable_if_t<std::is_same<weight_t_of<Aut>, bool>::value, Type>;

      /// Enable if Aut is not over Booleans.
      template <Automaton Aut, typename Type = void>
      using enable_if_not_boolean_t
        = std::enable_if_t<!std::is_same<weight_t_of<Aut>, bool>::value, Type>;

      /// Helper function to facilitate dispatch below.
      template <Automaton Aut, typename Tag, bool Lazy = false>
      automaton determinize_tag_(const Aut& aut)
      {
        return ::vcsn::determinize(aut, Tag{}, bool_constant<Lazy>{});
      }

      /// Boolean Bridge.
      template <Automaton Aut, typename String>
      enable_if_boolean_t<Aut, automaton>
      determinize_(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "determinization algorithm",
            {
              {"auto",          determinize_tag_<Aut, auto_tag>},
              {"boolean",       determinize_tag_<Aut, boolean_tag>},
              {"weighted",      determinize_tag_<Aut, weighted_tag>},
              {"lazy",          "lazy,auto"},
              {"lazy,auto",     "lazy,weighted"},
              {"lazy,weighted", determinize_tag_<Aut, weighted_tag, true>},
            }
          };
        return map[algo](aut->as<Aut>());
      }

      /// Weighted Bridge.
      template <Automaton Aut, typename String>
      enable_if_not_boolean_t<Aut, automaton>
      determinize_(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "determinization algorithm",
            {
              {"auto",          determinize_tag_<Aut, auto_tag>},
              {"weighted",      determinize_tag_<Aut, weighted_tag>},
              {"lazy",          "lazy,auto"},
              {"lazy,auto",     "lazy,weighted"},
              {"lazy,weighted", determinize_tag_<Aut, weighted_tag, true>},
            }
          };
        if (algo == "boolean")
          raise("determinize: cannot apply Boolean"
                " determinization to weighted automata");
        return map[algo](aut->as<Aut>());
      }

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      determinize(const automaton& aut, const std::string& algo)
      {
        return vcsn::detail::static_if<labelset_t_of<Aut>::is_free()>
          ([](const auto& aut, const auto& algo)
           { return determinize_<Aut, String>(aut, algo); },
           [](const auto&, const auto&) -> automaton
           { raise("determinize: requires free labelset"); })
          (aut, algo);
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
    auto res = transpose(determinize(transpose(aut), tag));
    res->properties().update(codeterminize_ftag{});
    return res;
  }

  /*---------------------.
  | dyn::codeterminize.  |
  `---------------------*/

  // FIXME: code duplication with determinize.
  namespace dyn
  {
    namespace detail
    {
      template <Automaton Aut, typename Tag>
      automaton codeterminize_tag_(const Aut& aut)
      {
        return ::vcsn::codeterminize(aut, Tag{});
      }

      /// Boolean Bridge.
      template <Automaton Aut, typename String>
      enable_if_boolean_t<Aut, automaton>
      codeterminize_(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "codeterminization algorithm",
            {
              {"auto",     codeterminize_tag_<Aut, auto_tag>},
              {"boolean",  codeterminize_tag_<Aut, boolean_tag>},
              {"weighted", codeterminize_tag_<Aut, weighted_tag>},
            }
          };
        return map[algo](aut->as<Aut>());
      }

      /// Weighted Bridge.
      template <Automaton Aut, typename String>
      enable_if_not_boolean_t<Aut, automaton>
      codeterminize_(const automaton& aut, const std::string& algo)
      {
        static const auto map = getarg<std::function<automaton(const Aut&)>>
          {
            "codeterminization algorithm",
            {
              {"auto",     codeterminize_tag_<Aut, auto_tag>},
              {"weighted", codeterminize_tag_<Aut, weighted_tag>},
            }
          };
        if (algo == "boolean")
          raise("codeterminize: cannot apply Boolean"
                " determinization to weighted automata");
        return map[algo](aut->as<Aut>());
      }

      /// Bridge.
      template <Automaton Aut, typename String>
      automaton
      codeterminize(const automaton& aut, const std::string& algo)
      {
        return vcsn::detail::static_if<labelset_t_of<Aut>::is_free()>
          ([](const auto& aut, const auto& algo)
           { return codeterminize_<Aut, String>(aut, algo); },
           [](const auto&, const auto&) -> automaton
           { raise("codeterminize: requires free labelset"); })
          (aut, algo);
      }
    }
  }
} // namespace vcsn
