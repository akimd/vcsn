#include <vcsn/misc/signature.hh>

#include <iostream>
#include <sstream>

namespace vcsn
{
  std::string
  signature::to_string() const
  {
    std::ostringstream os;
    os << *this;
    return os.str();
  }

  std::ostream&
  operator<<(std::ostream& os, const signature& sig)
  {
    const char* sep = "";
    for (auto s: sig.sig)
      {
        os << sep << s;
        sep = ", ";
      }
    return os;
  }
}
