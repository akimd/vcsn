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
      exp* parse_string(const std::string& e);
      const factory* factory_;

      void error(const location& l, const std::string& m);

    private:
      exp* parse();
    };

  }
}
#endif // ! VCSN_RAT_DRIVER_HH
