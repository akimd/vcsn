#ifndef VCSN_MISC_FADO_HH
# define VCSN_MISC_FADO_HH

# include <string>
# include <sstream>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace detail
  {
    class parse_fado
    {
    public:
      using automaton_t = dyn::automaton;

      parse_fado();
      automaton_t parse_file(const std::string& file);

    private:
      // Set context_ string
      std::string find_ctx(const std::string& file);
      automaton_t make_automata(const std::string& file);

      std::string context_;
      vcsn::automaton_editor *edit_;
      std::stringstream ss_;
    };
  }
}
#endif // VCSN_MISC_FADO_HH
