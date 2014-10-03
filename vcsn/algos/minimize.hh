#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/minimize-brzozowski.hh>
# include <vcsn/algos/minimize-moore.hh>
# include <vcsn/algos/minimize-signature.hh>
# include <vcsn/algos/minimize-weighted.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  namespace detail
  {
    template <typename Aut>
    constexpr bool can_use_brzozowski()
    {
      return labelset_t_of<Aut>::is_free()
        && std::is_same<weightset_t_of<Aut>, b>::value;
    }
  }

  // FIXME: there must exist some nicer way to do this.
  template <typename Aut>
  inline
  typename std::enable_if<std::is_same<weightset_t_of<Aut>, b>::value
                          && labelset_t_of<Aut>::is_free(),
                          partition_automaton<Aut>>::type
  minimize(const Aut& a, const std::string& algo)
  {
    if (algo == "moore")
      return minimize_moore(a);
    else if (algo == "signature")
      return minimize_signature(a);
    else if (algo == "weighted")
      return minimize_weighted(a);
    else
      raise("minimize: invalid algorithm (Boolean, free labelset): ",
            str_escape(algo));
  }

  template <typename Aut>
  inline
  typename std::enable_if<std::is_same<weightset_t_of<Aut>, b>::value
                          && ! labelset_t_of<Aut>::is_free(),
                          partition_automaton<Aut>>::type
  minimize(const Aut& a, const std::string& algo)
  {
    if (algo == "signature")
      return minimize_signature(a);
    else if (algo == "weighted")
      return minimize_weighted(a);
    else
      raise("minimize: invalid algorithm (Boolean, non-free labelset): ",
            str_escape(algo));
  }

  template <typename Aut>
  inline
  typename std::enable_if<!std::is_same<weightset_t_of<Aut>, b>::value,
                          partition_automaton<Aut>>::type
  minimize(const Aut& a, const std::string& algo)
  {
    if (algo == "weighted")
      return minimize_weighted(a);
    else
      raise("minimize: invalid algorithm (non-Boolean): ", str_escape(algo));
  }

  template <typename Aut>
  inline
  auto
  cominimize(const Aut& a, const std::string& algo)
    -> decltype(transpose(minimize(transpose(a), algo)))
  {
    return transpose(minimize(transpose(a), algo));
  }


  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut, typename String>
      inline
      typename std::enable_if<::vcsn::detail::can_use_brzozowski<Aut>(),
                              automaton>::type
      minimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "brzozowski")
          return make_automaton(::vcsn::minimize_brzozowski(a));
        else
          return make_automaton(::vcsn::minimize(a, algo));
      }

      template <typename Aut, typename String>
      inline
      typename std::enable_if<!::vcsn::detail::can_use_brzozowski<Aut>(),
                              automaton>::type
      minimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::minimize(a, algo));
      }

      REGISTER_DECLARE
      (minimize,
       (const automaton& aut, const std::string& algo) -> automaton);

    }
  }


  /*-----------------.
  | dyn::cominimize. |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut, typename String>
      inline
      typename std::enable_if<::vcsn::detail::can_use_brzozowski<Aut>(),
                              automaton>::type
      cominimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        if (algo == "brzozowski")
          return make_automaton(::vcsn::cominimize_brzozowski(a));
        else
          return make_automaton(::vcsn::cominimize(a, algo));
      }

      template <typename Aut, typename String>
      inline
      typename std::enable_if<!::vcsn::detail::can_use_brzozowski<Aut>(),
                              automaton>::type
      cominimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::cominimize(a, algo));
      }

      REGISTER_DECLARE
      (cominimize,
       (const automaton& aut, const std::string& algo) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_HH
