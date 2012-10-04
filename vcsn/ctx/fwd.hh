#ifndef VCSN_CTX_FWD_HH
# define VCSN_CTX_FWD_HH

namespace vcsn
{
  namespace ctx
  {
    class abstract_context;

    template <typename GenSet,
              typename WeightSet,
              typename Kind>
    class context;
  }
};

#endif // !VCSN_CTX_FWD_HH
