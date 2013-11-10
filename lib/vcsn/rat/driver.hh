#ifndef LIB_VCSN_RAT_DRIVER_HH
# define LIB_VCSN_RAT_DRIVER_HH

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <lib/vcsn/rat/location.hh>
# include <vcsn/misc/export.hh>

# include <lib/vcsn/rat/fwd.hh>
# include <lib/vcsn/rat/parse.hh>

namespace vcsn
{
  namespace rat
  {

    /// State and public interface for rational expression parsing.
    class LIBVCSN_API driver
    {
    public:
      driver(const dyn::ratexpset& rs);
      ~driver();
      /// Set the ratexpset to use.
      void ratexpset(const dyn::ratexpset& rs);
      /// Set the ratexpset to use from its context name.
      void context(const std::string& ctx);

      /// Parse this stream.
      dyn::ratexp parse(std::istream& is, const location& l = location{});

      /// Report an error \a m at \a l.
      void error(const location& l, const std::string& m);
      /// The string \a s is invalid at \a l.
      void invalid(const location& l, const std::string& s);

      /// The error messages.
      std::string errors;
      /// The scanner.
      std::unique_ptr<yyFlexLexer> scanner_;

    private:
      /// The inital location.
      location location_;
      dyn::ratexpset ratexpset_;
      vcsn::rat::exp_t result_;
      friend class parser;
    };

  }
}
#endif // !LIB_VCSN_RAT_DRIVER_HH
