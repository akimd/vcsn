#include <boost/algorithm/string.hpp>

#include <vcsn/dyn/context.hh>

namespace vcsn
{
  namespace dyn
  {
    std::string detail::context_base::sname(const std::string& vname)
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
      boost::replace_all(res, "lan_char", "lan<lal_char>");
      return res;
    }
  }
}
