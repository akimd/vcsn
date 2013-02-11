#ifndef VCSN_CTX_FWD_HH
# define VCSN_CTX_FWD_HH

# include <memory>

namespace vcsn
{
  // abstract_context.hh.
  namespace dyn
  {
    class abstract_context;
    using context = std::shared_ptr<const abstract_context>;
  }

  // ctx.hh.
  namespace ctx
  {
    template <typename LabelSet, typename WeightSet>
    class context;
  }
};

#endif // !VCSN_CTX_FWD_HH
