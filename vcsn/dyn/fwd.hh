#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <memory> // shared_ptr
# include <vcsn/core/rat/fwd.hh>

# include <vcsn/misc/export.hh> // LIBVCSN_API

namespace vcsn
{
  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class automaton_base;
    }
    using automaton = std::shared_ptr<detail::automaton_base>;

    // vcsn/dyn/context.hh.
    namespace detail
    {
      class abstract_context;
    }
    using context = std::shared_ptr<const detail::abstract_context>;

    // vcsn/dyn/polynomial.hh.
    namespace detail
    {
      class polynomial_base;
      template <typename PolynomialSet>
      class polynomial_wrapper;
    }

    using polynomial = std::shared_ptr<const detail::polynomial_base>;

    // vcsn/dyn/polynomialset.hh
    namespace detail
    {
      class polynomialset_base;
    }
    using polynomialset = std::shared_ptr<const detail::polynomialset_base>;

    // vcsn/dyn/ratexp.hh.
    namespace detail
    {
      class ratexp_base;
      template <typename RatExpSet>
      class ratexp_wrapper;
    }

    using ratexp = std::shared_ptr<detail::ratexp_base>;

    // vcsn/dyn/ratexpset.hh
    namespace detail
    {
      class ratexpset_base;
    }
    using ratexpset = std::shared_ptr<const detail::ratexpset_base>;

    // vcsn/dyn/weight.hh.
    namespace detail
    {
      class weight_base;
      template <typename T>
      class weight_wrapper;
    }
    using weight = std::shared_ptr<const detail::weight_base>;

    // weightset.hh.
    namespace detail
    {
      class weightset_base;
    }
    using weightset = std::shared_ptr<const detail::weightset_base>;

  }
} // namespace vcsn

// FIXME: Not the best place for this.
# define REGISTER_DECLARE(Name, Signature)                              \
  using Name ## _t = auto Signature;                                    \
  LIBVCSN_API                                                           \
  bool Name ## _register(const std::string& ctx, Name ## _t fn);

# define REGISTER_DECLARE2(Name, Signature)                             \
  using Name ## _t = auto Signature;                                    \
  LIBVCSN_API                                                           \
  bool Name ## _register(const std::string& ctx1, const std::string& ctx2, \
                         Name ## _t fn);

#endif // !VCSN_DYN_FWD_HH
