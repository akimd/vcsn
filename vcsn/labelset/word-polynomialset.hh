#pragma once

#include <vcsn/labelset/wordset.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  namespace detail
  {
    /*--------------------------.
    | make_word_polynomialset.  |
    `--------------------------*/

    template <typename Ctx>
    using word_polynomialset_t = polynomialset<word_context_t<Ctx>>;

    /// The polynomialset of words of a labelset (not necessarily on
    /// words itself).
    template <typename Ctx>
    inline auto
    make_word_polynomialset(const Ctx& ctx)
      -> word_polynomialset_t<Ctx>
    {
      return make_word_context(ctx);
    }
  }
}
