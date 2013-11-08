#ifndef LIB_VCSN_DOT_DRIVER_HH
# define LIB_VCSN_DOT_DRIVER_HH

# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/fwd.hh>
# include <vcsn/algos/edit-automaton.hh>
# include <vcsn/misc/export.hh>

# include <lib/vcsn/dot/fwd.hh>
# include <lib/vcsn/rat/location.hh>

namespace vcsn
{
  namespace detail
  {
    namespace dot
    {

      /// State and public interface for Dot parsing.
      class LIBVCSN_API driver
      {
      public:
        using location_t = vcsn::rat::location;

        driver();
        ~driver();

        /// Parse this stream.
        dyn::automaton parse(std::istream& is,
                             const location_t& l = location_t{});

        /// Report an error \a m at \a l.
        void error(const location_t& l, const std::string& m);
        /// The string \a s is invalid at \a l.
        void invalid(const location_t& l, const std::string& s);

        /// The error messages.
        std::string errors;
        /// The scanner.
        std::unique_ptr<detail_dotFlexLexer> scanner_;

      private:
        /// From context_, build edit_.
        /// \throw std::exception on invalid contexts.
        void setup_();

        /// The inital location.
        location_t location_;
        /// The name of the context.
        std::string context_;
        /// An automaton editor that stores the one being built.
        vcsn::automaton_editor* edit_;
        friend class parser;
      };

    }
  }
}
#endif // !LIB_VCSN_DOT_DRIVER_HH
