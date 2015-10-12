#pragma once

#include <iostream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/char_traits.hpp> // EOF, WOULD_BLOCK
#include <boost/iostreams/concepts.hpp>    // multichar_output_filter
#include <boost/iostreams/operations.hpp>  // get

namespace vcsn
{
  namespace detail
  {
    namespace io = boost::iostreams;

    /// Backslash backslashes.
    class backslashify_output_filter
      : public io::multichar_output_filter
    {
    public:
      explicit backslashify_output_filter()
      {}

      void enable()
      {
        enabled_ = true;
      }

      void disable()
      {
        enabled_ = false;
      }

      template <typename Sink>
      std::streamsize
      write(Sink& dest, const char* s, std::streamsize n)
      {
        std::streamsize z;
        for (z = 0; z < n; ++z)
          {
            char c = s[z];
            if (enabled_
                && (c == '\\' || c == '"'))
              if (!io::put(dest, '\\'))
                // FIXME: probably lost a char here.
                break;
            if (!io::put(dest, c))
              break;
          }
        return z;
      }

      bool enabled_ = false;
    };
  }
}
