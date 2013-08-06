#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <memory> // shared_ptr

namespace vcsn
{

  // vcsn/rat/ratexpset.hh
  class abstract_ratexpset;

  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class abstract_automaton;
    }
    using automaton = std::shared_ptr<detail::abstract_automaton>;

    // vcsn/dyn/context.hh.
    namespace detail
    {
      class abstract_context;
    }
    using context = std::shared_ptr<const detail::abstract_context>;

    // vcsn/dyn/ratexp.hh.
    namespace detail
    {
      class abstract_ratexp;
      template <typename RatExpSet>
      class concrete_abstract_ratexp;
    }

    using ratexp = std::shared_ptr<detail::abstract_ratexp>;

    // vcsn/dyn/ratexpset.hh
    namespace detail
    {
      class abstract_ratexpset;
    }
    using ratexpset = std::shared_ptr<const detail::abstract_ratexpset>;

    // vcsn/dyn/weight.hh.
    namespace detail
    {
      class abstract_weight;
      template <typename T>
      class concrete_abstract_weight;
    }
    using weight = std::shared_ptr<const detail::abstract_weight>;

    // weightset.hh.
    namespace detail
    {
      class abstract_weightset;
    }
    using weightset = std::shared_ptr<const detail::abstract_weightset>;

  }
} // namespace vcsn

// FIXME: Not the best place for this.
# define REGISTER_DECLARE(Name, Signature)                              \
  using Name ## _t = auto Signature;                                    \
  bool Name ## _register(const std::string& ctx, Name ## _t fn);

#endif // !VCSN_DYN_FWD_HH
