#include <sstream>
#include <fstream>
#include <getopt.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

#include <vcsn/labelset/fwd.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/algos/lightest.hh>
#include <vcsn/algos/lightest-automaton.hh>
#include <vcsn/algos/project-automaton.hh>
#include <vcsn/algos/partial-identity.hh>
#include <vcsn/algos/compose.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/algos/conjunction.hh>
#include <vcsn/algos/complement.hh>
#include <vcsn/algos/focus.hh>
#include <vcsn/algos/read-automaton.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/accessible.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/rmin.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/labelset/word-polynomialset.hh>

using letterset_t = vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>;
using null_letterset_t = vcsn::nullableset<letterset_t>;
using labelset_t = vcsn::tupleset<null_letterset_t, null_letterset_t>;
using context_t = vcsn::context<labelset_t, vcsn::rmin>;
using automaton_t = vcsn::mutable_automaton<context_t>;
using state_t = vcsn::state_t_of<automaton_t>;

// Context of the syntactical automaton. lan_char, rmin
using snd_context_t = vcsn::context<null_letterset_t, vcsn::rmin>;
using snd_automaton_t = vcsn::mutable_automaton<snd_context_t>;

/// Read an efsm automaton. Convert it to the requested type (Aut).
template <typename Aut>
Aut
read_automaton(const std::string& f)
{
  auto is = vcsn::open_input_file(f);
  std::cerr << "Reading: " << f << std::endl;
  return vcsn::read_automaton<Aut>(*is);
}

/// Sms2fr manipulates sentences in a particular format, format it back to
/// normal text: [#la#phrase#] -> la phrase.
std::string format(const std::string& str)
{
  using boost::replace_all_copy;
  using boost::replace_all;
  auto res = replace_all_copy(str.substr(2, str.size() - 4), "#", " ");
  replace_all(res, "\'", "\\\'");
  return res;
}

/// Create the unknown automaton used to re-insert ponctuation. Composed of
/// identity transition for known letters and the letter to epsilon for unknown
/// ones.
automaton_t sms_to_unk(const context_t& context)
{
  auto unk = vcsn::make_mutable_automaton(context);

  const auto& ls = *unk->labelset();

  state_t init_unk = unk->new_state();
  unk->set_initial(init_unk);

  const auto& ls0 = ls.template set<0>();
  auto generators0 = vcsn::detail::make_vector(ls0.generators());
  generators0.push_back(ls0.one());

  for (const auto l0: generators0)
    if (!ls0.is_one(l0) && (l0 == '#' || std::islower(l0)))
      unk->add_transition(init_unk, init_unk,
                                  ls.tuple(l0, l0), 0);

  unk->add_transition(init_unk, init_unk, ls.tuple('[', '['), 0);
  unk->add_transition(init_unk, init_unk, ls.tuple(']', ']'), 0);
  unk->set_final(init_unk);

  return unk;
}

/// Create the edit automata used to re-insert ponctuation. Composed of every
/// possible combination of letters (except with enclosing characters: brackets).
automaton_t sms_to_edit(const context_t& context)
{
  auto edit = vcsn::make_mutable_automaton(context);

  state_t init = edit->new_state();
  edit->set_initial(init);

  const auto& ls = *edit->labelset();
  const auto& ls0 = ls.template set<0>();
  const auto& ls1 = ls.template set<1>();
  auto generators0 = vcsn::detail::make_vector(ls0.generators());
  auto generators1 = vcsn::detail::make_vector(ls1.generators());
  generators0.push_back(ls0.one());
  generators1.push_back(ls1.one());

  for (const auto l0: generators0)
    for (const auto l1: generators1)
      if ((!ls0.is_one(l0) || !ls1.is_one(l1))
          && l0 != '[' && l1 != '['
          && l0 != ']' && l1 != ']')
        edit->add_transition(init, init,
                             ls.tuple(l0, l1),
                             (l0 == l1) ? 0 : 1);

  edit->add_transition(init, init, ls.tuple('[', '['), 0);
  edit->add_transition(init, init, ls.tuple(']', ']'), 0);
  edit->set_final(init);

  return edit;
}

/// The sms automaton is the automaton accepting the original text message,
/// changed to this format: '[#my#text#message#]'. Hence, the characters
/// ('#', '[' and ']') are handled as special characters in the trained automata
/// and not accepted in the original text. We create the sms automaton as a
/// double tape automaton for future composition.
automaton_t sms_to_aut(const context_t& context, automaton_t& unk,
                       automaton_t& edit, std::string& line)
{
  auto res = vcsn::make_mutable_automaton(context);
  const auto& ls = *res->labelset();

  const auto& ls0 = edit->labelset()->template set<0>();

  state_t init = res->new_state();
  res->set_initial(init);
  state_t snd = res->new_state();
  res->add_transition(init, snd, ls.tuple('[', '['), 0);
  state_t trd = res->new_state();
  res->add_transition(snd, trd, ls.tuple('#', '#'), 0);

  size_t p = 0;
  state_t prev = trd;

  while (p < line.length())
    {
      state_t s = res->new_state();
      auto letter = line[p];
      if (isspace(letter))
        letter = '#';
      res->add_transition(prev, s, ls.tuple(letter, letter), 0);
      if (!ls0.is_valid(letter) || (letter != '#' && letter < 'a' || 'z' < letter))
        {
          auto unk_state = unk->dst_of(initial_transitions(unk).front());
          unk->add_transition(unk_state, unk_state,
                              unk->labelset()->tuple(letter, ls0.one()), 0);
        }
      if (!ls0.is_valid(letter))
        {
          auto edit_state = edit->dst_of(initial_transitions(edit).front());
          edit->add_transition(edit_state, edit_state,
                               edit->labelset()->tuple(letter, letter), 0);
        }
      p++;
      prev = s;
    }
  state_t sep = res->new_state();
  res->add_transition(prev, sep, ls.tuple('#', '#'), 0);
  state_t end = res->new_state();
  res->add_transition(sep, end, ls.tuple(']', ']'), 0);
  res->set_final(end);

  return res;
}

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

  std::string graphemic_file;
  std::string syntactic_file;
  bool prompt = true;
};

int main(int argc, char* argv[])
{
  auto opts = options{argc, argv};

  // Read the graphemic automaton.
  auto grap = read_automaton<automaton_t>(opts.graphemic_file);
  // Read the syntactic automaton (partial identity for composition).
  auto synt = vcsn::partial_identity(read_automaton<snd_automaton_t>(opts.syntactic_file));

  std::string sms;
  automaton_t unknown_aut = sms_to_unk(grap->context());
  automaton_t edit_aut = sms_to_edit(grap->context());

  if (opts.prompt)
    std::cout << "sms > ";

  while (getline(std::cin, sms))
    {
      // Create the automaton corresponding to the sms.
      automaton_t sms_aut
        = sms_to_aut(grap->context(), unknown_aut, edit_aut, sms);

      // Remove unknown letters.
      auto kill_unk = vcsn::compose(sms_aut, unknown_aut);

      auto aut_p = vcsn::proper(kill_unk);

      // First composition with the graphemic automaton.
      auto aut_g
        = vcsn::strip(vcsn::coaccessible(vcsn::compose(sms_aut, grap)));

      // Second composition with the syntactic automaton.
      auto aut_s = vcsn::strip(vcsn::coaccessible(vcsn::compose(aut_g, synt)));

      // Prepare automaton for lightest.
      auto aut_s_proper = vcsn::proper(vcsn::project<1>(aut_s));
      auto aut_s_null = make_nullable_automaton(aut_s_proper->context());
      copy_into(aut_s_proper, aut_s_null);

      // Retrieve the path more likely (automaton is weighted) to correspond
      // to the translation in actual french.
      auto lightest_aut
        = vcsn::partial_identity(vcsn::lightest_automaton(aut_s_null, 1));

      // Add possibility to insert unknown letters between each letter.
      auto add_unk = vcsn::compose<decltype(lightest_aut),
                                   decltype(unknown_aut), 1, 1>(
                       lightest_aut,
                       unknown_aut
                     );

      // All possible editions of the original sms automaton.
      auto edit_sms = vcsn::compose(sms_aut, edit_aut);

      // Intersection between the possible editions and the translation with
      // possible ponctuation.
      auto edited = vcsn::conjunction(
                      vcsn::project<1>(add_unk),
                      vcsn::sort(vcsn::project<1>(edit_sms))
                    );

      auto edited_proper = vcsn::proper(edited);

      // Retrieve the path likeliest (automaton is weighted) to correspond
      // to the text with ponctuation.
      auto lightest = vcsn::lightest(vcsn::project<1>(lightest_aut), 1);
      // Print the result.
      for (const auto& m: lightest)
        std::cout << format(m.first) << '\n';

      if (opts.prompt)
        std::cout << "sms > ";
    }
}
