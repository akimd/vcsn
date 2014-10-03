/**
 ** \file lib/vcsn/misc/indent.cc
 ** \brief Implementation of indentation relative functions.
 **/

#include <cassert>
#include <iomanip>
#include <ostream>

#include <vcsn/misc/indent.hh>
#include <vcsn/misc/xalloc.hh>

namespace vcsn
{

  namespace detail
  {
    xalloc<long int> indentation;
  }

  std::ostream& incindent(std::ostream& o)
  {
    detail::indentation(o) += 2;
    return o;
  }

  std::ostream& decindent(std::ostream& o)
  {
    assert(detail::indentation(o));
    detail::indentation(o) -= 2;
    return o;
  }

  std::ostream& resetindent(std::ostream& o)
  {
    detail::indentation(o) = 0;
    return o;
  }

  std::ostream& indent(std::ostream& o)
  {
    // Be sure to be able to restore the stream flags.
    char fill = o.fill(' ');
    return o << std::setw(detail::indentation(o))
             << ""
             << std::setfill(fill);
  }

  std::ostream& iendl(std::ostream& o)
  {
    return o << '\n' << indent;
  }

  std::ostream& incendl(std::ostream& o)
  {
    return o << incindent << iendl;
  }

  std::ostream& decendl(std::ostream& o)
  {
    return o << decindent << iendl;
  }
}
