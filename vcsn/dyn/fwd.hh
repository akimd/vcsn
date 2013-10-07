#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <memory> // shared_ptr
# include <vcsn/core/rat/fwd.hh>

namespace vcsn
{
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

    // vcsn/dyn/polynomial.hh.
    namespace detail
    {
      class abstract_polynomial;
      template <typename PolynomialSet>
      class concrete_abstract_polynomial;
    }

    using polynomial = std::shared_ptr<const detail::abstract_polynomial>;

    // vcsn/dyn/polynomialset.hh
    namespace detail
    {
      class abstract_polynomialset;
    }
    using polynomialset = std::shared_ptr<const detail::abstract_polynomialset>;

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

    /// A dyn::ratexpset from its static ratexpset.
    // Should remain part of vcsn/dyn/ratexpset.hh, but we have a
    // circular dependency to break (dyn::ratexpset obviously needs
    // vcsn/core/rat/ratexpset.hh, and rat::ratexpset::conv needs
    // dyn::make_ratexpset).
    template <typename Context>
    ratexpset make_ratexpset(const vcsn::ratexpset<Context>& rs);

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

# define REGISTER_DECLARE2(Name, Signature)                             \
  using Name ## _t = auto Signature;                                    \
  bool Name ## _register(const std::string& ctx1, const std::string& ctx2, \
                         Name ## _t fn);

#endif // !VCSN_DYN_FWD_HH
