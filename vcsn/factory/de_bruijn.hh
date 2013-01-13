#ifndef VCSN_FACTORY_DE_BRUIJN_HH
# define VCSN_FACTORY_DE_BRUIJN_HH

# include <iterator>
# include <stdexcept>

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  // (a+b)*a(a+b)^n.
  template <class Context>
  mutable_automaton<Context>
  de_bruijn(unsigned n, const Context& ctx)
  {
    static_assert(Context::is_lal,
                  "requires labels_are_letters");
    size_t sz =
      std::distance(std::begin(*ctx.labelset()), std::end(*ctx.labelset()));
    if (sz < 2)
      throw std::invalid_argument("de_bruijn: the alphabet needs"
                                  " at least 2 letters");
    using context_t = Context;
    mutable_automaton<context_t> res{ctx};

    auto init = res.new_state();
    res.set_initial(init);
    for (char l: *ctx.labelset())
      res.set_transition(init, init, l);

    auto prev = res.new_state();
    res.set_transition(init, prev, *std::begin(*ctx.labelset()));

    while (n--)
      {
        auto next = res.new_state();
        for (char l: *ctx.labelset())
          res.set_transition(prev, next, l);
        prev = next;
      }
    res.set_final(prev);
    return res;
  }

  /*-----------------.
  | dyn::de_bruijn.  |
  `-----------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Ctx>
      automaton
      de_bruijn(const dyn::context& ctx, unsigned n)
      {
        return std::make_shared<mutable_automaton<Ctx>>
          (de_bruijn<Ctx>(n, dynamic_cast<const Ctx&>(*ctx)));
      }

      using de_bruijn_t =
        auto (const dyn::context& ctx, unsigned n) -> automaton;

      bool de_bruijn_register(const std::string& ctx, const de_bruijn_t& fn);
    }
  }

}

#endif // !VCSN_FACTORY_DE_BRUIJN_HH
