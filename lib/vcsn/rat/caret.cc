#include <lib/vcsn/rat/caret.hh>

#include <fstream>

namespace vcsn
{
  namespace detail
  {
    namespace
    {
      void print_caret_impl(std::istream& is,
                            std::ostream& os, const rat::location& loc)
      {
        const auto pos = is.tellg();
        is.seekg(0);
        // Look for the right line.
        auto buf = std::string{};
        auto line = 1u;
        while (getline(is, buf) && line != loc.begin.line)
          ++line;
        // If it's on several lines, just display the first one.
        auto end_col
          = loc.begin.line == loc.end.line
          ? loc.end.column
          : buf.size() + 1;
        os << '\n'
           << buf
           << '\n'
           << std::string(loc.begin.column - 1, ' ')
           << std::string(std::max(1ul, end_col - loc.begin.column), '^');
        // Restore position.
        is.seekg(pos);
      }
    }

    void print_caret(std::istream& is,
                     std::ostream& os, const rat::location& loc)
    {
      print_caret_impl(is, os, loc);
    }
  }
}
