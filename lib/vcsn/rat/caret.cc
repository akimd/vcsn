#include <lib/vcsn/rat/caret.hh>

#include <istream>
#include <sstream>
#include <ostream>

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
        // If we hit EOF, the stream is bad.  Clear that so that we
        // can seekg etc.
        is.clear();
        is.seekg(0);
        // Look for the right line.
        auto buf = std::string{};
        for (auto line = 1u; getline(is, buf) && line != loc.begin.line; ++line)
          continue;
        // If it's on several lines, just display the first one.
        auto end_col
          = loc.begin.line == loc.end.line
          ? loc.end.column
          : buf.size() + 1;
        os << buf
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
      os << '\n';
      print_caret_impl(is, os, loc);
    }

    std::string caret(std::istream& is, const rat::location& loc)
    {
      auto os = std::ostringstream{};
      print_caret_impl(is, os, loc);
      return os.str();
    }
  }
}
