#include <lib/vcsn/algos/fwd.hh>
#include <lib/vcsn/algos/registry.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/rat/read.hh> // rat::read
#include <vcsn/algos/read.hh>
#include <vcsn/core/rat/expressionset.hh> // make_expressionset
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/registries.hh>
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
#if 1
      static const auto map
        = getarg<std::function<automaton(std::istream&)>>
        {
          "automaton input format",
          {
            {"default", "dot"},
            {"daut",    read_daut},
            {"dot",     read_dot},
            {"efsm",    read_efsm},
            {"fado",    read_fado},
          }
        };
      auto res = map[f](is);
      return strip_p ? strip(res) : res;
#endif
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
