#ifndef VCSN_CTX_CHAR_B_LAL_HH
# define VCSN_CTX_CHAR_B_LAL_HH

# include <vcsn/ctx/char.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using char_b_lal = char_<vcsn::b, labels_are_letters>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(char_b_lal);

  MAYBE_EXTERN template
  auto
  determinize(const mutable_automaton<ctx::char_b_lal>& aut)
    -> mutable_automaton<ctx::char_b_lal>;

  MAYBE_EXTERN template
  class details::evaluator<mutable_automaton<ctx::char_b_lal>>;

  MAYBE_EXTERN template
  bool
  eval(const mutable_automaton<ctx::char_b_lal>& aut, const std::string& w);

# if VCSN_INSTANTIATION
  namespace
  {
    namespace details
    {
      template <typename Ctx>
      bool
      register_functions()
      {
        using aut_t = mutable_automaton<Ctx>;
        using namespace dyn::details;

        de_bruijn_register(Ctx::sname(), de_bruijn<Ctx>);
        determinize_register(aut_t::sname(), determinize<aut_t>);
        eval_register(aut_t::sname(), eval<aut_t>);
        return true;
      }

      static bool register_char_b_lal = register_functions<ctx::char_b_lal>();
    }
  }
#endif
};

#endif // !VCSN_CTX_CHAR_B_LAL_HH
