#include <vcsn/misc/stream.hh>

#include <cassert>
#include <istream>

namespace vcsn
{

  std::string
  bracketed(std::istream& i, const char lbracket, const char rbracket)
  {
    assert(i.peek() == lbracket);
    i.ignore();
    size_t level = 1;
    std::ostringstream o;
    while (true)
      {
        if (i.peek() == lbracket)
          ++level;
        else if (i.peek() == rbracket
                 && !--level)
          {
            i.ignore();
            break;
          }
        o << char(i.get());
      }
    return o.str();
  }
}
