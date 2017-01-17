#include <sstream>
#include <fstream>
#include <getopt.h>

#include <boost/algorithm/string/replace.hpp>

#include <vcsn/dyn/value.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/core/rat/identities.hh>

namespace vcsn
{
  namespace dyn
  {
    static automaton read_automaton(const std::string& f)
    {
      auto is = open_input_file(f);
      return read_automaton(*is, "efsm");
    }
  }
}

/// Sms2fr manipulates sentences in a particular format, format it back to
/// normal text: [#la#phrase#] -> la phrase.
std::string format(const vcsn::dyn::polynomial& lightest)
{
  auto str = vcsn::dyn::format(lightest);
  auto begin = str.find('#');
  auto end = str.rfind('#');
  using boost::algorithm::replace_all_copy;
  return replace_all_copy(str.substr(begin + 1, end - begin - 1), "#", " ");
}

int main(int argc, char* argv[])
{
  std::string graphemic_file;
  std::string syntactic_file;
  bool prompt = true;

  { /// Options
    struct option longopts[] =
    {
      {"graphemic", required_argument,  NULL, 'g'},
      {"syntactic", required_argument,  NULL, 's'},
      {"no-prompt", no_argument,        0,    'n'},
      {0, 0, 0, 0}
    };
    int opti;
    char opt;

    while ((opt = getopt_long(argc, argv, "g:s:", longopts, &opti)) != EOF)
      {
        switch(opt)
        {
          case 'g': // --graphemic
            graphemic_file = std::string(optarg);
            break;
          case 's': // --syntactic
            syntactic_file = std::string(optarg);
            break;
          case 'n': // Do not display the prompt.
            prompt = false;
            break;
          default:
            std::cerr << opt << " : unkown option ! \n";
            return -1;
        }
      }
    vcsn::require(!graphemic_file.empty() && !syntactic_file.empty(),
                  "Either graphemic or syntactic file not given.\n");
  }

  // Read the graphemic automaton.
  auto grap = vcsn::dyn::read_automaton(graphemic_file);
  // Read the syntactic automaton (partial identity for composition).
  auto synt = vcsn::dyn::partial_identity(vcsn::dyn::read_automaton(syntactic_file));

  std::string sms;
  if (prompt)
    std::cout << "sms > ";

  auto ctx = vcsn::dyn::make_context("lan_char, rmin");
  while (getline(std::cin, sms))
    {
      // The sms automaton is the automaton accepting the original text message,
      // changed to this format: '[#my#text#message#]'. Partial identity for
      // future composition.
      std::replace(begin(sms), end(sms), ' ', '#');
      std::stringstream ss;
      ss << "\\[#" << sms << "#\\]";
      std::istringstream iss(ss.str());
      auto sms_exp = vcsn::dyn::read_expression(ctx, {}, iss);
      auto sms_aut = vcsn::dyn::to_automaton(sms_exp);
      auto aut_p = vcsn::dyn::partial_identity(sms_aut);

      // First composition with the graphemic automaton.
      auto aut_g = vcsn::dyn::strip(vcsn::dyn::coaccessible(vcsn::dyn::compose(aut_p, grap)));

      // Second composition with the syntactic automaton.
      auto aut_s = vcsn::dyn::strip(vcsn::dyn::coaccessible(vcsn::dyn::compose(aut_g, synt)));

      // Prepare automaton for lightest.
      auto aut_s_out = vcsn::dyn::project(aut_s, 1);
      auto aut_s_proper = vcsn::dyn::proper(aut_s_out);

      // Retrieve the path more likely (automaton is weighted) to correspond
      // to the translation in actual french. Then print it.
      auto lightest = vcsn::dyn::lightest(aut_s_proper, 1);
      if (prompt)
        std::cout << lightest << '\n';

      std::cout << format(lightest) << '\n';

      if (prompt)
        std::cout << "sms > ";
    }
}
