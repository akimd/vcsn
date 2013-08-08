#ifndef LIB_VCSN_RAT_DRIVER_HH
# define LIB_VCSN_RAT_DRIVER_HH

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <lib/vcsn/rat/location.hh>

namespace vcsn
{
  namespace rat
  {

    /// State and public interface for rational expression parsing.
    class driver
    {
    public:
      driver() = default;
      driver(const dyn::ratexpset& rs);
      /// Set the context to use.
      void ratexpset(const dyn::ratexpset& rs);

      dyn::ratexp parse_file(const std::string& f);
      dyn::ratexp parse_string(const std::string& e,
                               const location& l = location());

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
      dyn::ratexp parse_(const location& l = location{});
      /// Close the scanner.
      void scan_close_();

      /// The inital location.
      location location_;
      dyn::ratexpset ratexpset_;
      vcsn::rat::exp_t result_;
      friend class parser;
    };

  }
}
#endif // !LIB_VCSN_RAT_DRIVER_HH
