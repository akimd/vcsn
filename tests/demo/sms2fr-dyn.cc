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
/// normal text: "[#la#phrase#]" -> "la phrase".
std::string format(const vcsn::dyn::polynomial& lightest)
{
  auto str = vcsn::dyn::format(lightest);
  auto begin = str.find('#');
  auto end = str.rfind('#');
  using boost::algorithm::replace_all_copy;
  return replace_all_copy(str.substr(begin + 1, end - begin - 1), "#", " ");
}

struct sms2fr_impl
{
  std::string operator()(const std::string& sms) const
  {
    // The sms automaton is the automaton accepting the original text
    // message, changed to this format: '[#my#text#message#]'.  Partial
    // identity for future composition.
    auto s = "\\[#" + boost::replace_all_copy(sms, " ", "#") +  "#\\]";
    auto sms_exp = vcsn::dyn::make_expression(ctx, s);
    auto sms_aut = vcsn::dyn::to_automaton(sms_exp);
    auto aut_p = vcsn::dyn::partial_identity(sms_aut);

    // First composition with the graphemic automaton.
    auto aut_g =
      vcsn::dyn::strip
      (vcsn::dyn::coaccessible
       (vcsn::dyn::compose(aut_p, grap)));

    // Second composition with the syntactic automaton.
    auto aut_s =
      vcsn::dyn::strip
      (vcsn::dyn::coaccessible
       (vcsn::dyn::compose(aut_g, synt)));

    // Prepare automaton for lightest.
    auto aut_s_out = vcsn::dyn::project(aut_s, 1);
    auto aut_s_proper = vcsn::dyn::proper(aut_s_out);

    // Retrieve the path more likely (automaton is weighted) to correspond
    // to the translation in actual french. Then print it.
    auto lightest = vcsn::dyn::lightest(aut_s_proper, 1);
    return format(lightest);
  }

  const vcsn::dyn::automaton &grap;
  const vcsn::dyn::automaton &synt;
  const vcsn::dyn::context ctx = vcsn::dyn::make_context("lan_char, rmin");
};

/// Command line arguments.
struct options
{
  options(int argc, char* argv[])
  {
    /// Options
    struct option longopts[] =
    {
      {"graphemic", required_argument,  nullptr, 'g'},
      {"syntactic", required_argument,  nullptr, 's'},
      {"no-prompt", no_argument,        nullptr, 'n'},
      {nullptr, 0, nullptr, 0}
    };

    int opti;
    char opt;

    while ((opt = getopt_long(argc, argv, "g:s:", longopts, &opti)) != EOF)
      {
        switch(opt)
          {
          case 'g': // --graphemic
            graphemic_file = optarg;
            break;
          case 's': // --syntactic
            syntactic_file = optarg;
            break;
          case 'n': // Do not display the prompt.
            prompt = false;
            break;
          default:
            vcsn::raise("invalid option: ", opt);
          }
      }
    vcsn::require(!graphemic_file.empty(),
                  "graphemic file not specified");
    vcsn::require(!syntactic_file.empty(),
                  "syntactic file not specified");
  }

  std::string datafile(const std::string& f)
  {
    auto datadir = vcsn::dyn::configuration("configuration.datadir");
    return datadir + "/sms2fr/" + f + ".efsm";
  }

  std::string graphemic_file = datafile("graphemic");
  std::string syntactic_file = datafile("syntactic");
  bool prompt = true;
};

int main(int argc, char* argv[])
{
  auto opts = options{argc, argv};

  // Read the graphemic automaton.
  auto graphemic = vcsn::dyn::read_automaton(opts.graphemic_file);
  // Read the syntactic automaton (partial identity for composition).
  auto syntactic = vcsn::dyn::partial_identity
    (vcsn::dyn::read_automaton(opts.syntactic_file));

  auto sms2fr = sms2fr_impl{graphemic, syntactic};

  std::string sms;
  if (opts.prompt)
    std::cout << "sms > ";

  while (getline(std::cin, sms))
    {
      std::cout << sms2fr(sms) << '\n';
      if (opts.prompt)
        std::cout << "sms > ";
    }
}
