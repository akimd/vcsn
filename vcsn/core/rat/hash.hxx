#include <vcsn/misc/functional.hh>

namespace vcsn
{
  namespace rat
  {

#define DEFINE                                  \
    template <typename ExpSet>                  \
    auto                                        \
    hash<ExpSet>

#define VISIT(Type)                           \
    DEFINE::visit(const Type ## _t& v)        \
      -> void


#undef VISIT
#undef DEFINE

  } // namespace rat
} // namespace vcsn
