#ifndef VCSN_ALGOS_INFO_HH
# define VCSN_ALGOS_INFO_HH

# include <iostream>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/is-complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/is-eps-acyclic.hh>
# include <vcsn/algos/is-normalized.hh>
# include <vcsn/algos/is-valid.hh>
# include <vcsn/algos/standard.hh>
# include <vcsn/algos/synchronizing-word.hh>
# include <vcsn/core/rat/info.hh>
# include <vcsn/core/rat/size.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace detail_info
  {
    /*---------------.
    | is-ambiguous.  |
    `---------------*/
    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::is_free(),
                            bool>::type
    is_ambiguous(const Aut& a)
    {
      return vcsn::is_ambiguous(a);
    }

    template <typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::is_free(),
                            std::string>::type
    is_ambiguous(const Aut&)
    {
      return "N/A";
    }

    /*--------------.
    | is-complete.  |
    `--------------*/
    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::is_free(),
                            bool>::type
    is_complete(const Aut& a)
    {
      return vcsn::is_complete(a);
    }

    template <typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::is_free(),
                            std::string>::type
    is_complete(const Aut&)
    {
      return "N/A";
    }

    /*-------------------.
    | is_deterministic.  |
    `-------------------*/
    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::is_free(),
                            bool>::type
    is_deterministic(const Aut& a)
    {
      return vcsn::is_deterministic(a);
    }

    template <typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::is_free(),
                            std::string>::type
    is_deterministic(const Aut&)
    {
      return "N/A";
    }

    /*-------------------.
    | is_synchronizing.  |
    `-------------------*/
    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::is_free(),
                            bool>::type
    is_synchronizing(const Aut& a)
    {
      return vcsn::is_synchronizing(a);
    }

    template <typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::is_free(),
                            std::string>::type
    is_synchronizing(const Aut&)
    {
      return "N/A";
    }

    /*---------------------------.
    | num_deterministic_states.  |
    `---------------------------*/
    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::is_free(),
                            size_t>::type
    num_deterministic_states(const Aut& a)
    {
      return vcsn::num_deterministic_states(a);
    }

    template <typename Aut>
    typename std::enable_if<!labelset_t_of<Aut>::is_free(),
                            std::string>::type
    num_deterministic_states(const Aut&)
    {
      return "N/A";
    }

    /*----------------------.
    | num_eps_transitions.  |
    `----------------------*/

    template <typename Aut>
    ATTRIBUTE_CONST
    typename std::enable_if<!labelset_t_of<Aut>::has_one(),
                            size_t>::type
    num_eps_transitions_(const Aut&)
    {
      return 0;
    }

    template <typename Aut>
    typename std::enable_if<labelset_t_of<Aut>::has_one(),
                            size_t>::type
    num_eps_transitions_(const Aut& aut)
    {
      size_t res = 0;
      for (auto t : aut->transitions())
        res += aut->labelset()->is_one(aut->label_of(t));
      return res;
    }

    template <typename Aut>
    size_t
    num_eps_transitions(const Aut& aut)
    {
      return num_eps_transitions_(aut);
    }
  }

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <class A>
  std::ostream&
  info(const A& aut, std::ostream& out, bool detailed = false)
  {
#define ECHO(Name, Value)                               \
    out << Name ": " << Value << '\n'
    ECHO("type", aut->vname(true));
    ECHO("number of states", aut->num_states());
    ECHO("number of initial states", aut->num_initials());
    ECHO("number of final states", aut->num_finals());
    ECHO("number of accessible states", num_accessible_states(aut));
    ECHO("number of coaccessible states", num_coaccessible_states(aut));
    ECHO("number of useful states", num_useful_states(aut));
    ECHO("number of transitions", aut->num_transitions());
    ECHO("number of deterministic states",
         detail_info::num_deterministic_states(aut));
    ECHO("number of eps transitions", detail_info::num_eps_transitions(aut));
    if (detailed)
      ECHO("is ambiguous", detail_info::is_ambiguous(aut));
    ECHO("is complete", detail_info::is_complete(aut));
    ECHO("is deterministic", detail_info::is_deterministic(aut));
    ECHO("is empty", is_empty(aut));
    ECHO("is eps-acyclic", is_eps_acyclic(aut));
    ECHO("is normalized", is_normalized(aut));
    ECHO("is proper", is_proper(aut));
    ECHO("is standard", is_standard(aut));
    if (detailed)
      ECHO("is synchronizing", detail_info::is_synchronizing(aut));
    ECHO("is trim", is_trim(aut));
    ECHO("is useless", is_useless(aut));
#undef ECHO
    // No eol for the last one.
    out << "is valid: " << is_valid(aut);
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream, typename Bool>
      std::ostream& info(const automaton& aut, std::ostream& out,
                         bool detailed)
      {
        info(aut->as<Aut>(), out, detailed);
        return out;
      }

      REGISTER_DECLARE(info,
                       (const automaton& aut, std::ostream& out,
                        bool detailed) -> std::ostream&);
    }
  }


  /*-----------------------.
  | info(ratexp, stream).  |
  `-----------------------*/

  template <class RatExpSet>
  void
  info(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
       std::ostream& o)
  {
    rat::size<RatExpSet> sizer;
    vcsn::rat::info<RatExpSet> nfo;
    nfo(*e);

# define DEFINE(Type)                            \
    << "\n" #Type ": " << nfo.Type
    o
      << "type: " << rs.vname(true)
      << "\nsize: " << sizer(e)
      DEFINE(sum)
      DEFINE(shuffle)
      DEFINE(conjunction)
      DEFINE(prod)
      DEFINE(star)
      DEFINE(complement)
      DEFINE(zero)
      DEFINE(one)
      DEFINE(atom)
      DEFINE(lweight)
      DEFINE(rweight)
      ;
# undef DEFINE
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Ostream>
      std::ostream& info_ratexp(const ratexp& exp, std::ostream& o)
      {
        const auto& e = exp->as<RatExpSet>();
        vcsn::info<RatExpSet>(e.ratexpset(), e.ratexp(), o);
        return o;
      }

      REGISTER_DECLARE(info_ratexp,
                        (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_INFO_HH
