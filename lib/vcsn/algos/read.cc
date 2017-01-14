#include <map>
#include <regex>

#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/algos/guess-automaton-format.hh>
#include <vcsn/core/rat/expressionset.hh> // make_expressionset
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
    using r = std::regex;
    // Probes for each mode.
    const static auto probes = std::multimap<std::string, std::regex>
      {
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
              try
                {
                  is.seekg(pos);
                }
              catch (const std::ios_base::failure& e)
                {
                  raise("cannot rewing automaton file: ", e.what());
                }
              return p.first;
            }
      }
    is.seekg(pos);
    raise("cannot guess automaton format: ", is);
  }

  namespace dyn
  {
    /*-----------------.
    | read_automaton.  |
    `-----------------*/

    namespace
    {
      automaton read_auto(std::istream& is)
      {
        return read_automaton(is, guess_automaton_format(is), false);
      }

      automaton read_dot(std::istream& is)
      {
        vcsn::detail::dot::driver d;
        auto res = d.parse(is);
        if (!d.errors.empty())
          raise(d.errors);
        return res;
      }
    }

    automaton
    read_automaton(std::istream& is, const std::string& f,
                   bool strip_p)
    {
      static const auto map
        = getarg<std::function<automaton(std::istream&)>>
        {
          "automaton input format",
          {
            {"auto",    read_auto},
            {"default", "auto"},
            {"daut",    read_daut},
            {"dot",     read_dot},
            {"efsm",    read_efsm},
            {"fado",    read_fado},
          }
        };
      auto res = map[f](is);
      return strip_p ? strip(res) : res;
    }

    /*-------------------.
    | read_expression.   |
    `-------------------*/

    expression
    read_expression(const context& ctx, rat::identities ids,
                    std::istream& is, const std::string& f)
    {
      using fun_t = auto (const context&, rat::identities,
                          std::istream&) -> expression;
      static const auto map = getarg<std::function<fun_t>>
        {
          "expression input format",
          {
            {"default", "text"},
            {"text",    [](const context& ctx, rat::identities ids,
                           std::istream& is) {
                return rat::read(ctx, ids, is);
              }},
          }
        };
      return map[f](ctx, ids, is);
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
