#ifndef VCSN_DYN_TRANSLATE_HH
# define VCSN_DYN_TRANSLATE_HH

# include <string>

# include <vcsn/misc/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    /// Compile, and load, a DSO with instantiations for \a ctx.
    void compile(const std::string& ctx);

    /// Compile, and load, a DSO which instantiates \a algo for \a sig.
    void compile(const std::string& algo, const signature& sig);

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_TRANSLATE_HH
