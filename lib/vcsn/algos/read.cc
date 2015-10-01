#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/core/rat/expressionset.hh> // make_expressionset
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  namespace dyn
  {

    /*-----------------.
    | read_automaton.  |
    `-----------------*/

    namespace
    {
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
      enum fmt
      {
        dot,
        efsm,
        fado,
      };
      static const auto map
        = std::map<std::string, std::function<automaton(std::istream&)>>
        {
          {"default", read_dot},
          {"dot",     read_dot},
          {"efsm",    read_efsm},
          {"fado",    read_fado},
        };
      auto read = getargs("automaton input format", map, f);
      auto res = read(is);
      return strip_p ? strip(res) : res;
    }

    /*-------------------.
    | read_expression.   |
    `-------------------*/

    expression
    read_expression(const context& ctx, rat::identities ids,
                    std::istream& is, const std::string& f)
    {
      enum fmt
      {
        text,
      };
      static const auto map = std::map<std::string, fmt>
        {
          {"default", text},
          {"text",    text},
        };
      switch (getargs("expression input format", map, f))
        {
        case text:
          return rat::read(ctx, ids, is);
        }
      BUILTIN_UNREACHABLE();
    }

    /*-------------.
    | read_label.  |
    `-------------*/

    REGISTRY_DEFINE(read_label);
    label
    read_label(const dyn::context& ctx, std::istream& is,
               const std::string& f)
    {
      enum fmt
      {
        text,
        quoted
      };
      static const auto map = std::map<std::string, fmt>
        {
          {"default", text},
          {"text",    text},
          {"quoted",  quoted},
        };
      auto format = getargs("label input format", map, f);
      bool is_quoted = format == quoted;
      return detail::read_label_registry().call(ctx, is, is_quoted);
    }
  }
} // vcsn::
