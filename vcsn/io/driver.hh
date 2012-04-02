#ifndef VCSN_RAT_DRIVER_HH
# define VCSN_RAT_DRIVER_HH

# include <vcsn/core/rat/node.hh>
# include <vcsn/core/rat/factory.hh>
# include <vcsn/io/location.hh>

namespace vcsn
{
  namespace rat
  {

    /// State and public interface for rational expression parsing.
    class driver
    {
    public:
      driver(const factory& f);
      exp* parse_file(const std::string& f);
      exp* parse_string(const std::string& e, const location& l = location());
      const factory* factory_;

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
      exp* parse(const location& l = location());
      /// Close the scanner.
      void scan_close();

      exp* result_;
      friend class parser;
    };

  }
}
#endif // ! VCSN_RAT_DRIVER_HH
