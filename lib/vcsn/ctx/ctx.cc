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
          case ' ':
            // Recognize 'lat<lal_char(ab), lal_char(xy)>_lat<q, r>'
            // as 'lat<lal_char(ab),lal_char(xy)>_lat<q,r>': ignore
            // spaces.
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
