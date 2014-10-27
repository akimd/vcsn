#ifndef VCSN_DYN_FWD_HH
# define VCSN_DYN_FWD_HH

# include <vcsn/misc/export.hh> // LIBVCSN_API
# include <vcsn/misc/fwd.hh>
# include <vcsn/misc/memory.hh> // make_shared_ptr

namespace vcsn
{
  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    namespace detail
    {
      class automaton_base;
      template <typename Aut>
      class automaton_wrapper;
    }
    using automaton = std::shared_ptr<detail::automaton_base>;

    // vcsn/dyn/context.hh.
    namespace detail
    {
      class context_base;
      template <typename Context>
      class context_wrapper;
    }
    using context = std::shared_ptr<const detail::context_base>;

    // vcsn/dyn/expansion.hh.
    namespace detail
    {
      class expansion_base;
      template <typename Aut>
      class expansion_wrapper;
    }
    using expansion = std::shared_ptr<const detail::expansion_base>;

    // vcsn/dyn/label.hh.
    namespace detail
    {
      class label_base;
      template <typename T>
      class label_wrapper;
    }
    using label = std::shared_ptr<const detail::label_base>;

    // vcsn/dyn/polynomial.hh.
    namespace detail
    {
      class polynomial_base;
      template <typename PolynomialSet>
      class polynomial_wrapper;
    }
    using polynomial = std::shared_ptr<const detail::polynomial_base>;

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
      template <typename RatExpSet>
      class ratexpset_wrapper;
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
  }

  namespace ast
  {
    class ast_node;
    class automaton;
    class context;
    class genset;
    class letterset;
    class nullableset;
    class oneset;
    class other;
    class polynomialset;
    class ratexpset;
    class tupleset;
    class weightset;
    class wordset;
  }
} // namespace vcsn

// FIXME: Not the best place for this.
# define REGISTER_DECLARE(Name, Signature)                      \
  using Name ## _t = auto Signature;                            \
  LIBVCSN_API                                                   \
  bool Name ## _register(const signature& sig, Name ## _t fn)

#endif // !VCSN_DYN_FWD_HH
