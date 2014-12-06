#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <stack>
# include <string>
# include <type_traits>
# include <queue>

# include <vcsn/algos/transpose.hh>
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>
# include <vcsn/misc/map.hh> // vcsn::has
# include <vcsn/misc/raise.hh> // b
# include <vcsn/misc/unordered_map.hh> // vcsn::has
# include <vcsn/weightset/fwd.hh> // b
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*----------------------.
  | subset construction.  |
  `----------------------*/
  namespace detail
  {
    /// \brief The subset construction automaton from another.
    ///
    /// \tparam Aut an automaton type.
    /// \pre labelset is free.
    /// \pre weightset is Boolean.
    template <typename Aut>
    class determinized_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: boolean: requires free labelset");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "determinize: boolean: requires Boolean weights");

    public:
      using automaton_t = Aut;
      using automaton_nocv_t = typename Aut::element_type::automaton_nocv_t;
      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using super_t = automaton_decorator<automaton_nocv_t>;

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
        map_[n] = super_t::pre();
        todo_.push(n);

        // Final states.
        for (auto t : input_->final_transitions())
          finals_.set(input_->src_of(t));
      }

      static symbol sname()
      {
        static symbol res("determinized_automaton<"
                          + automaton_t::element_type::sname() + '>');
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "determinized_automaton<";
        input_->print_set(o, format);
        return o << '>';
      }

      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_name_t& ss)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(ss);
        if (i == std::end(map_))
          {
            res = this->new_state();
            map_[ss] = res;

            if (ss.intersects(finals_))
              this->set_final(res);

            todo_.push(ss);
          }
        else
          res = i->second;
        return res;
      }

      /// Determinize all accessible states.
      void operator()()
      {
        std::map<label_t, state_name_t, vcsn::less<labelset_t_of<Aut>>> ml;
        while (!todo_.empty())
          {
            auto ss = std::move(todo_.top());
            state_t src = state(ss);
            todo_.pop();

            ml.clear();
            for (auto s = ss.find_first(); s != ss.npos;
                 s = ss.find_next(s))
              {
                // Cache the output transitions of state s.
                auto i = successors_.find(s);
                if (i == successors_.end())
                  {
                    i = successors_.emplace(s, label_map_t{}).first;
                    auto& j = i->second;
                    for (auto t : input_->out(s))
                      {
                        auto l = input_->label_of(t);
                        if (j.find(l) == j.end())
                          j[l].resize(state_size_);
                        j[l].set(input_->dst_of(t));
                      }
                  }

                // Store in ml the possible destination per label.
                for (const auto& p : i->second)
                  {
                    auto j = ml.find(p.first);
                    if (j == ml.end())
                      ml[p.first] = p.second;
                    else
                      j->second |= p.second;
                  }
              }

            // Outgoing transitions from the current (result) state.
            for (const auto& e : ml)
              this->new_transition(src, state(e.second), e.first);
          }
      }

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& format = "text",
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
                input_->print_state_name(s, o, format, true);
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
              std::set<state_t> from;
              const auto& ss = p.first;
              for (auto s = ss.find_first(); s != ss.npos;
                   s = ss.find_next(s))
                from.emplace(s);
              origins_.emplace(p.second, std::move(from));
            }
        return origins_;
      }

    private:
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
      using stack = std::stack<state_name_t>;
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
  template <typename Aut>
  using determinized_automaton
    = std::shared_ptr<detail::determinized_automaton_impl<Aut>>;

  template <typename Aut>
  inline
  auto
  determinize(const Aut& a)
    -> determinized_automaton<Aut>
  {
    auto res = make_shared_ptr<determinized_automaton<Aut>>(a);
    // Determinize.
    res->operator()();
    return res;
  }

  template <typename Aut>
  inline
  auto
  codeterminize(const Aut& a)
    -> decltype(transpose(determinize(transpose(a))))
  {
    return transpose(determinize(transpose(a)));
  }

  /*---------------------------.
  | weighted determinization.  |
  `---------------------------*/
  namespace detail
  {
    /// \brief The weighted determinization of weighted automaton.
    ///
    /// \tparam Aut an weighted automaton type.
    /// \pre labelset is free.
    template <typename Aut>
    class detweighted_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "determinize: weighted: requires free labelset");

    public:
      using automaton_t = Aut;
      using automaton_nocv_t = typename Aut::element_type::automaton_nocv_t;
      using super_t = automaton_decorator<automaton_nocv_t>;

      using label_t = label_t_of<automaton_t>;
      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;

      using state_t = state_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      /// An output state is a list of weighted input states.
      struct stateset
      {
        stateset(const automaton_t& aut)
          : aut_(aut)
        {}

        using value_t = state_t;
        using kind_t = void;
        static bool equals(state_t l, state_t r)
        {
          return l == r;
        }

        static bool less(state_t l, state_t r)
        {
          return l < r;
        }

        static size_t hash(state_t s)
        {
          return hash_value(s);
        }

        std::ostream&
        print(state_t s, std::ostream& out,
              const std::string& format = "text") const
        {
          return aut_->print_state_name(s, out, format);
        }

        automaton_t aut_;
      };

      using state_nameset_t = polynomialset<context<stateset, weightset_t>>;
      using state_name_t = typename state_nameset_t::value_t;

      /// Build the weighted determinizer.
      /// \param a         the weighted automaton to determinize
      detweighted_automaton_impl(const automaton_t& a)
        : super_t(a->context())
        , input_(a)
      {
        // Pre.
        state_name_t n;
        n.emplace(input_->pre(), ws_.one());
        map_[n] = super_t::pre();
        todo_.push(n);

        // Post.
        n.clear();
        n.emplace(input_->post(), ws_.one());
        map_[n] = super_t::post();
      }

      static symbol sname()
      {
        static symbol res("detweighted_automaton<"
                          + automaton_t::element_type::sname() + '>');
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "detweighted_automaton<";
        input_->print_set(o, format);
        return o << '>';
      }

      /// The determinization of weighted automaton
      /// with the idea based on Mohri's algorithm.
      void operator()()
      {
        // label -> <destination, sum of weights>.
        std::map<label_t, state_name_t,
                 vcsn::less<labelset_t_of<automaton_t>>> dests;
        while (!todo_.empty())
          {
            auto ss = std::move(todo_.front());
            todo_.pop();
            auto src = map_[ss];

            dests.clear();
            for (const auto& p : ss)
              {
                auto s = p.first;
                auto v = p.second;
                for (auto t : input_->all_out(s))
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
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(state_t ss, std::ostream& o,
                       const std::string& format = "text") const
      {
        auto i = origins().find(ss);
        if (i == origins().end())
          this->print_state(ss, o);
        else
          ns_.print(i->second, o, format, ", ");
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
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(name);
        if (i == std::end(map_))
          {
            res = this->new_state();
            map_[name] = res;
            todo_.push(name);
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
      state_nameset_t ns_ = {{stateset(input_), ws_}};

      /// The sets of (input) states waiting to be processed.
      using queue = std::queue<state_name_t>;
      queue todo_;
    };
  }

  /// A determinized automaton as a shared pointer.
  template <typename Aut>
  using detweighted_automaton
    = std::shared_ptr<detail::detweighted_automaton_impl<Aut>>;

  template <typename Aut>
  inline
  auto
  determinize_weighted(const Aut& a)
    -> detweighted_automaton<Aut>
  {
    auto res = make_shared_ptr<detweighted_automaton<Aut>>(a);
    res->operator()();
    return res;
  }

  template <typename Aut>
  inline
  auto
  codeterminize_weighted(const Aut& aut)
    -> decltype(transpose(determinize_weighted(transpose(aut))))
  {
    return transpose(determinize_weighted(transpose(aut)));
  }

  /*-------------------.
  | dyn::determinize.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut, typename Type>
      using if_boolean_t
        = typename std::enable_if<std::is_same<weightset_t_of<Aut>, b>::value,
                                  Type>::type;

      template <typename Aut, typename Type>
      using if_not_boolean_t
        = typename std::enable_if<!std::is_same<weightset_t_of<Aut>, b>::value,
                                  Type>::type;


      /// Boolean Bridge.
      template <typename Aut, typename String>
      if_boolean_t<Aut, automaton>
      determinize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "auto" || algo == "boolean")
          return make_automaton(::vcsn::determinize(a));
        else if (algo == "weighted")
          return make_automaton(::vcsn::determinize_weighted(a));
        else
          raise("determinize: invalid algorithm: ", str_escape(algo));
      }

      /// Weighted Bridge.
      template <typename Aut, typename String>
      if_not_boolean_t<Aut, automaton>
      determinize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "boolean")
          raise("determinize: cannot apply Boolean determinization");
        else if (algo == "auto" || algo == "weighted")
          return make_automaton(::vcsn::determinize_weighted(a));
        else
          raise("determinize: invalid algorithm: ", str_escape(algo));
      }

      REGISTER_DECLARE(determinize,
                       (const automaton& aut, const std::string& algo)
                       -> automaton);
    }
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
      template <typename Aut, typename String>
      if_boolean_t<Aut, automaton>
      codeterminize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "auto" || algo == "boolean")
          return make_automaton(::vcsn::codeterminize(a));
        else if (algo == "weighted")
          return make_automaton(::vcsn::codeterminize_weighted(a));
        else
          raise("codeterminize: invalid algorithm: ", str_escape(algo));
      }

      /// Weighted Bridge.
      template <typename Aut, typename String>
      if_not_boolean_t<Aut, automaton>
      codeterminize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "boolean")
          raise("codeterminize: cannot apply Boolean determinization");
        else if (algo == "auto" || algo == "weighted")
          return make_automaton(::vcsn::codeterminize_weighted(a));
        else
          raise("codeterminize: invalid algorithm: ", str_escape(algo));
      }

      REGISTER_DECLARE(codeterminize,
                       (const automaton& aut, const std::string& algo)
                       -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
