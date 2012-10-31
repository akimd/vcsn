#ifndef LIB_VCSN_RAT_DRIVER_HH
# define LIB_VCSN_RAT_DRIVER_HH

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/rat/fwd.hh>
# include <lib/vcsn/rat/location.hh>

namespace vcsn
{
  namespace rat
  {

    /// State and public interface for rational expression parsing.
    class driver
    {
    public:
      using exp_t = vcsn::rat::exp_t;
      driver(const abstract_kratexpset& f);
      driver(const dyn::context& ctx);
      exp_t parse_file(const std::string& f);
      exp_t parse_string(const std::string& e, const location& l = location());

      /// Report an error \a m at \a l.
      void error(const location& l, const std::string& m);
      /// The string \a s is invalid at \a l.
      void invalid(const location& l, const std::string& s);

      /// The error messages.
      std::string errors;

    private:
      /// Prepare scanner to load file f.
      void scan_open_(FILE *f);
      /// Prepare scanner to read string e.
      void scan_open_(const std::string& e);
      /// Parse this stream.
      exp_t parse_(const location& l = location{});
      /// Close the scanner.
      void scan_close_();

      /// The inital location.
      location location_;
      const abstract_kratexpset* kratexpset_;
      exp_t result_;
      friend class parser;
    };

  }
}
#endif // !LIB_VCSN_RAT_DRIVER_HH
