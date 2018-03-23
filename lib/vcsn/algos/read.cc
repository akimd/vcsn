#include <map>
#include <regex>

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/read.hh> // detail::dot::read
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/algos/guess-automaton-format.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  std::string
  guess_automaton_format(std::istream& is)
  {
    const auto pos = is.tellg();
    require(pos != -1,
            "cannot keep file position while guessing automaton file format");
    using r = std::regex;
    // Probes for each mode.
    const static auto probes = std::multimap<std::string, std::regex>
      {
        {"efsm.bzip2",  r{"^BZh"}},
        {"efsm.lzma",   r{"\xFD""7zXZ""\x00"}},
        {"daut",  r{"^\\s*context *="}},
        {"daut",  r{"^\\s*(\\$|\\w+|\".*?\")\\s*->\\s*(\\$|\\w+|\".*?\")"}},
        {"dot",   r{"^\\s*digraph"}},
        {"efsm",  r{"^#! /bin/sh"}},
        {"fado",  r{"^@([DN]FA|Transducer) "}},
        {"grail", r{"\\(START\\)"}},
      };
    const auto daut = std::regex();
    while (is.good())
      {
        std::string line;
        std::getline(is, line, '\n');
        for (const auto& p: probes)
          if (std::regex_search(line, p.second))
            {
              is.seekg(pos);
              require(is.good(), "cannot rewind automaton file");
              return p.first;
            }
      }
    raise("cannot guess automaton format: ", is);
  }

  namespace dyn
  {
    /*-----------------.
    | read_automaton.  |
    `-----------------*/

    namespace
    {
      automaton read_auto(std::istream& is, const location& loc)
      {
        return read_automaton(is, guess_automaton_format(is), false, loc);
      }
    }

    automaton
    read_automaton(std::istream& is, const std::string& f,
                   bool strip_p, const location& loc)
    {
      static const auto map
        = getarg<std::function<automaton(std::istream&, const location& loc)>>
        {
          "automaton input format",
          {
            {"auto",       read_auto},
            {"daut",       read_daut},
            {"default",    "auto"},
            {"dot",        ::vcsn::detail::dot::read},
            {"efsm",       read_efsm},
            {"efsm.bzip2", read_efsm_bzip2},
            {"efsm.lzma",  read_efsm_lzma},
            {"fado",       read_fado},
          }
        };
      auto res = map[f](is, loc);
      return strip_p ? strip(res) : res;
    }

    /*-------------------.
    | read_expression.   |
    `-------------------*/

    expression
    read_expression(const context& ctx, rat::identities ids,
                    std::istream& is, const std::string& f,
                    const location& loc)
    {
      return rat::read(ctx, ids, is, f, loc);
    }

    /*-------------.
    | read_label.  |
    `-------------*/

    REGISTRY_DEFINE(read_label);
    label
    read_label(const dyn::context& ctx, std::istream& is,
               const std::string& f)
    {
      static const auto map = getarg<bool>
        {
          "label input format",
          {
            // name,     quoted
            {"default", "text"},
            {"text",    false},
            {"quoted",  true},
          }
        };
      // Lvalue needed by dyn::.
      bool is_quoted = map[f];
      return detail::read_label_registry().call(ctx, is, is_quoted);
    }
  }
} // vcsn::
