#include <vcsn/ctx/context.hh>

namespace vcsn
{
  namespace dyn
  {
    std::string detail::abstract_context::sname(const std::string& vname)
    {
      std::string res;
      int nesting = 0;
      for (char c: vname)
        switch (c)
          {
          case '(':
            ++nesting;
            break;
          case ')':
            --nesting;
            break;
          default:
            if (!nesting)
              res += c;
            break;
          }
      return res;
    }
  }
}
