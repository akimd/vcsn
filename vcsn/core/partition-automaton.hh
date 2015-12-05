#pragma once

#include <map>
#include <set>
#include <vector>

#include <vcsn/algos/transpose.hh>
#include <vcsn/core/automaton-decorator.hh>

namespace vcsn
{
  namespace detail
  {

    /// An automaton wrapper whose states form a partition of the
    /// state set of another automaton.
    ///
    /// \tparam Aut the type of the wrapped input automaton.
    template <typename Aut>
    class partition_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
    public:
      /// Input automaton type.
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      /// Generated automaton type.
      template <typename Ctx = context_t>
      using fresh_automaton_t = fresh_automaton_t_of<automaton_t, Ctx>;
      using label_t = label_t_of<automaton_t>;
      using super_t = automaton_decorator<fresh_automaton_t<>>;

      /// The underlying state type.
      using state_t = state_t_of<automaton_t>;

      /// The state names: a set of the original automaton states.
      using state_name_t = std::set<state_t>;

      /// A map from each state to the origin state set it stands for.
      using origins_t = std::map<state_t, state_name_t>;

      partition_automaton_impl(const automaton_t& input)
        : super_t(input->context())
        , input_(input)
      {
        origins_[super_t::pre()]  = {input_->pre()};
        origins_[super_t::post()] = {input_->post()};
      }

      partition_automaton_impl(const fresh_automaton_t<>& res,
                               const automaton_t& input,
                               const origins_t& origins)
        : super_t(res)
        , input_(input)
        , origins_(origins)
      {
        origins_[super_t::pre()]  = {input_->pre()};
        origins_[super_t::post()] = {input_->post()};
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"partition_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "partition_automaton<";
        input_->print_set(o, fmt);
        return o << '>';
      }

      bool state_has_name(state_t) const
      {
        return true;
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool delimit = false) const
      {
        const auto& set = origins_.at(s);
        const char* separator = "";
        if (delimit)
          o << '{';
        for (auto s : set)
          {
            o << separator;
            input_->print_state_name(s, o, fmt, true);
            separator = ", ";
          }
        if (delimit)
          o << '}';
        return o;
      }

      /// Accessor to the states' origins.
      const origins_t& origins() const
      {
        return origins_;
      }

      /// Accessor to the input automaton.
      const automaton_t& input_automaton() const
      {
        return input_;
      }

      using super_t::new_state;

      /// Make a new state representing the given input state set,
      /// which is required to be new -- no error-checking on this.
      state_t new_state(const state_name_t& set)
      {
        state_t res = new_state();
        origins_[res] = set;
        return res;
      }

      state_t new_state(const std::vector<state_t>& v)
      {
        return new_state(state_name_t{begin(v), end(v)});
      }

    private:
      /// The input automaton.
      const automaton_t input_;
      /// A map from each state to the origin state set it stands for.
      origins_t origins_;
    }; // partition_automaton_impl
  } // namespace detail

  /// A partition automaton as a shared pointer.
  template <typename Aut>
  using partition_automaton
    = std::shared_ptr<detail::partition_automaton_impl<Aut>>;



  namespace detail
  {
    /// From an (input) automaton type, compute its origin_t type.
    template <typename Aut>
    struct origins_t_of_impl;

    /// The type of the origins map for a partition automaton, or a
    /// transposed one.
    template <typename Aut>
    using origins_t_of = typename origins_t_of_impl<Aut>::type;

    template <typename Aut>
    struct origins_t_of_impl<partition_automaton<Aut>>
    {
      using type = typename partition_automaton<Aut>::element_type::origins_t;
    };

    template <typename Aut>
    struct origins_t_of_impl<transpose_automaton<Aut>>
    {
      using type = origins_t_of<Aut>;
    };



    /// From an (input) automaton type, compute the right decorator
    /// for its partition_automaton.  For instance,
    /// partition_automaton<partition_automaton<Aut>> =>
    /// partition_automaton<Aut>.
    ///
    /// we don't want to stack partition_automaton, so that we can
    /// minimize (and cominimize) repeatedly without changing the type
    /// of the automaton).
    template <typename Aut>
    struct partition_automaton_t_impl
    {
      using type = partition_automaton<Aut>;
    };

    template <typename Aut>
    struct partition_automaton_t_impl<partition_automaton<Aut>>
      : partition_automaton_t_impl<Aut>
    {};

    template <typename Aut>
    struct partition_automaton_t_impl<transpose_automaton<Aut>>
    {
      using type
        = transpose_automaton<typename partition_automaton_t_impl<Aut>::type>;
    };
  }

  /// The return type when calling quotient on Aut.
  template <typename Aut>
  using partition_automaton_t
    = typename detail::partition_automaton_t_impl<Aut>::type;


  /// Build a partition_automaton.
  ///
  /// \param res     the actual result, typically a mutable_automaton
  /// \param input   the automaton from which the partition is computed
  /// \param origins map each state of res to its states in input
  template <typename Aut>
  auto
  make_partition_automaton(const fresh_automaton_t_of<Aut>& res,
                           const Aut& input,
                           const typename detail::partition_automaton_impl<Aut>::origins_t origins)
    -> partition_automaton_t<Aut>
  {
    return make_shared_ptr<partition_automaton<Aut>>(res, input, origins);
  }

  /// Build a partition_automaton of a partition_automaton:
  /// smash into a single level partition_automaton.
  ///
  /// \param res     the actual result, typically a mutable_automaton
  /// \param input   the automaton from which the partition is computed
  /// \param origins map each state of res to its states in input
  template <typename Aut>
  auto
  make_partition_automaton(const fresh_automaton_t_of<Aut>& res,
                           const partition_automaton<Aut>& input,
                           const typename detail::partition_automaton_impl<Aut>::origins_t origins)
    -> partition_automaton_t<Aut>
  {
    const auto& input_origins = input->origins();
    using origins_t
      = typename detail::partition_automaton_impl<Aut>::origins_t;
    auto new_origins = origins_t{};

    for (const auto& p: origins)
      for (auto s: p.second)
        new_origins[p.first]
          .insert(begin(input_origins.at(s)), end(input_origins.at(s)));

    return make_partition_automaton(res, input->input_automaton(), new_origins);
  }

  /// Build a partition_automaton of a transpose_automaton:
  /// smash into a transpose_automaton of a partition_automaton.
  ///
  /// \param res     the actual result, typically a mutable_automaton
  /// \param input   the automaton from which the partition is computed
  /// \param origins map each state of res to its states in input
  template <typename Aut>
  auto
  make_partition_automaton(const fresh_automaton_t_of<transpose_automaton<Aut>>& res,
                           const transpose_automaton<Aut>& input,
                           const typename detail::partition_automaton_impl<Aut>::origins_t origins)
    -> transpose_automaton<partition_automaton_t<Aut>>
  {
    return transpose(make_partition_automaton(res->naked_automaton(),
                                              input->naked_automaton(),
                                              origins));
  }

} // namespace vcsn
