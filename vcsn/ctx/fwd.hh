#ifndef VCSN_CTX_FWD_HH
# define VCSN_CTX_FWD_HH

# include <memory>

namespace vcsn
{
  namespace dyn
  {
    class abstract_context;
    using context = std::shared_ptr<const abstract_context>;
  }

  namespace ctx
  {
    template <typename LabelSet,
              typename WeightSet,
              typename Kind>
    class context;
  }
};

#endif // !VCSN_CTX_FWD_HH
