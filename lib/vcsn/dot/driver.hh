#ifndef LIB_VCSN_DOT_DRIVER_HH
# define LIB_VCSN_DOT_DRIVER_HH

# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/fwd.hh>
# include <vcsn/algos/edit-automaton.hh>

# include <lib/vcsn/rat/location.hh>

namespace vcsn
{
  namespace detail
  {
    namespace dot
    {

      /// State and public interface for Dot parsing.
      class driver
      {
      public:
        using automaton_t = dyn::automaton;
        using location_t = vcsn::rat::location;

        driver();

        automaton_t parse_file(const std::string& f);
        automaton_t parse_string(const std::string& e,
                                 const location_t& l = location_t{});

        /// Report an error \a m at \a l.
        void error(const location_t& l, const std::string& m);
        /// The string \a s is invalid at \a l.
        void invalid(const location_t& l, const std::string& s);

        /// The error messages.
        std::string errors;

      private:
        /// From context_, build edit_.
        /// \throw std::exception on invalid contexts.
        void setup_();
        /// Prepare scanner to load file f.
        void scan_open_(FILE *f);
        /// Prepare scanner to read string e.
        void scan_open_(const std::string& e);
        /// Parse this stream.
        automaton_t parse_(const location_t& l = location_t{});
        /// Close the scanner.
        void scan_close_();

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
