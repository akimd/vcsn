#ifndef LIB_VCSN_DOT_DRIVER_HH
# define LIB_VCSN_DOT_DRIVER_HH

# include <lib/vcsn/dot/location.hh>

namespace vcsn
{
  namespace dot
  {

    /// State and public interface for Dot parsing.
    class driver
    {
    public:
      using exp_t = void;
      driver();
      exp_t parse_file(const std::string& f);
      exp_t parse_string(const std::string& e, const location& l = location());
      /// Report an error \a m at \a l.
      void error(const location& l, const std::string& m);
      /// The string \a s is invalid at \a l.
      void invalid(const location& l, const std::string& s);

      /// The error messages.
      std::string errors;

      /// The inital location.
      location location_;

    private:
      /// Prepare scanner to load file f.
      void scan_open(FILE *f);
      /// Prepare scanner to read string e.
      void scan_open(const std::string& e);
      /// Parse this stream.
      void parse(const location& l = location());
      /// Close the scanner.
      void scan_close();

      friend class parser;
    };

  }
}
#endif // !LIB_VCSN_DOT_DRIVER_HH
