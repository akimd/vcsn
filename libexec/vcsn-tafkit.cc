#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh>

#include "parse-args.hh"

#define DEFINE_AUT_FUNCTION(Name)               \
  struct Name: vcsn_function                    \
  {                                             \
    int work_aut(const options& opts) const     \
    {                                           \
      using namespace vcsn::dyn;                \
      /* Input. */                              \
      auto aut = read_automaton(opts);          \
                                                \
      /* Process. */                            \
      auto res = vcsn::dyn::Name(aut);          \
                                                \
      /* Output. */                             \
      opts.print(res);                          \
      return 0;                                 \
    }                                           \
  }

#define DEFINE_AUT_VARIADIC_FUNCTION(Name)              \
  struct Name: vcsn_function                            \
  {                                                     \
    int work_aut(const options& opts) const             \
    {                                                   \
      using namespace vcsn::dyn;                        \
      /* Input. */                                      \
      auto res = read_automaton(opts);                  \
      for (unsigned i = 0; i < opts.argv.size(); ++i)   \
        {                                               \
          /* Hack. */                                   \
          options opts2 = opts;                         \
          opts2.input = opts.argv[i];                   \
          automaton rhs = read_automaton(opts2);        \
          res = vcsn::dyn::Name(res, rhs);              \
        }                                               \
                                                        \
      /* Output. */                                     \
      opts.print(res);                                  \
      return 0;                                         \
    }                                                   \
  }

#define DEFINE_EXP_FUNCTION(Name)               \
  struct Name: vcsn_function                    \
  {                                             \
    int work_exp(const options& opts) const     \
    {                                           \
      using namespace vcsn::dyn;                \
      /* Input. */                              \
      auto exp = read_expression(opts);         \
                                                \
      /* Process. */                            \
      auto res = vcsn::dyn::Name(exp);          \
                                                \
      /* Output. */                             \
      opts.print(res);                          \
      return 0;                                 \
    }                                           \
  }

#define DEFINE_AUT_EXP_FUNCTION(Name)           \
  struct Name: vcsn_function                    \
  {                                             \
    int work_aut(const options& opts) const     \
    {                                           \
      using namespace vcsn::dyn;                \
      automaton aut = read_automaton(opts);     \
                                                \
      auto res = vcsn::dyn::Name(aut);          \
                                                \
      opts.print(res);                          \
      return 0;                                 \
    }                                           \
                                                \
    int work_exp(const options& opts) const     \
    {                                           \
      using namespace vcsn::dyn;                \
      expression exp = read_expression(opts);   \
                                                \
      auto res = vcsn::dyn::Name(exp);          \
                                                \
      opts.print(res);                          \
      return 0;                                 \
    }                                           \
  }

#define DEFINE_INT_INT_FUNCTION(Name)                           \
  static int Name(int argc, char* const argv[])                 \
  {                                                             \
    options opts;                                               \
    opts.input_format = "text";                                 \
    parse_args(opts, argc, argv);                               \
                                                                \
    /* Input. */                                                \
    using namespace vcsn::dyn;                                  \
    auto ctx = make_context(opts.context);                      \
    vcsn::require(2 <= argc,                                    \
                  "vcsn " #Name ": too few arguments: ",        \
                  argc, ", expected 2");                        \
    auto k = boost::lexical_cast<unsigned>(argv[0]);            \
    auto b = boost::lexical_cast<unsigned>(argv[1]);            \
                                                                \
    /* Process. */                                              \
    automaton aut = Name(ctx, k, b);                            \
                                                                \
    /* Output. */                                               \
    opts.print(aut);                                            \
    return 0;                                                   \
  }

#define DEFINE(Function, Signature)             \
  DEFINE_ ## Signature ## _FUNCTION(Function)

DEFINE(accessible, AUT);
DEFINE(add, AUT_VARIADIC);
DEFINE(coaccessible, AUT);
DEFINE(complement, AUT);
DEFINE(complete, AUT);
DEFINE(compose, AUT_VARIADIC);
DEFINE(constant_term, EXP);
DEFINE(difference, AUT_VARIADIC);
DEFINE(divkbaseb, INT_INT);
DEFINE(expand, EXP);
DEFINE(inductive, EXP);
DEFINE(infiltrate, AUT_VARIADIC);
DEFINE(is_ambiguous, AUT);
DEFINE(is_complete, AUT);
DEFINE(is_deterministic, AUT);
DEFINE(is_empty, AUT);
DEFINE(is_eps_acyclic, AUT);
DEFINE(is_normalized, AUT);
DEFINE(is_proper, AUT);
DEFINE(is_standard, AUT);
DEFINE(is_trim, AUT);
DEFINE(is_useless, AUT);
DEFINE(is_valid, AUT_EXP);
DEFINE(lift, AUT_EXP);
DEFINE(multiply, AUT_VARIADIC);
DEFINE(proper, AUT);
DEFINE(quotkbaseb, INT_INT);
DEFINE(shuffle, AUT_VARIADIC);
DEFINE(split, EXP);
DEFINE(standard, AUT_EXP);
DEFINE(star, AUT);
DEFINE(star_normal_form, EXP);
DEFINE(thompson, EXP);
DEFINE(to_expression, AUT);
DEFINE(transpose, AUT_EXP);
DEFINE(trim, AUT);
DEFINE(universal, AUT);

struct are_equivalent: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton lhs = read_automaton(opts);
    // Hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    automaton rhs = read_automaton(opts2);

    // Process.
    auto res = vcsn::dyn::are_equivalent(lhs, rhs);

    // Output.
    opts.print(res);
    return res ? 0 : 2;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    expression lhs = read_expression(opts);
    // Hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    expression rhs = read_expression(opts2);

    // Process.
    auto res = vcsn::dyn::are_equivalent(lhs, rhs);

    // Output.
    opts.print(res);
    return res ? 0 : 2;
  }
};

struct are_isomorphic: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton lhs = read_automaton(opts);
    // Hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    automaton rhs = read_automaton(opts2);

    // FIXME: generalize to an arbitrary number of automata

    // Process.
    auto res = vcsn::dyn::are_isomorphic(lhs, rhs);

    // Output.
    opts.print(res);
    return res ? 0 : 2;
  }
};

struct cat: vcsn_function
{
  virtual int work_aut(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    // Output.
    opts.print(aut);
    return 0;
  }

  virtual int work_exp(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_expression(opts);
    // Output.
    opts.print(exp);
    return 0;
  }

  virtual int work_polynomial(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_polynomial(opts);
    // Output.
    opts.print(exp);
    return 0;
  }

  virtual int work_weight(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto w = read_weight(opts);
    // Output.
    opts.print(w);
    return 0;
  }
};

struct conjunction: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    /* Input. */
    std::vector<automaton> as;
    as.emplace_back(read_automaton(opts));
    for (unsigned i = 0; i < opts.argv.size(); ++i)
      {
        options opts2 = opts;
        opts2.input = opts.argv[i];
        as.emplace_back(read_automaton(opts2));
      }
    auto res = vcsn::dyn::conjunction(as);

    /* Output. */
    opts.print(res);
    return 0;
  }
};

struct derivation: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    expression exp = read_expression(opts);
    vcsn::require(0 < opts.argv.size(),
                  "vcsn derivation: too few arguments: ",
                  opts.argv.size(), ", expected 2");
    options opts2 = opts;
    opts2.input = opts.argv[0];
    opts2.input_is_file = false;
    vcsn::dyn::label l = read_word(opts2);
    bool breaking = (1 < opts.argv.size()
                     ? boost::lexical_cast<bool>(opts.argv[1])
                     : false);

    // Process.
    auto res = vcsn::dyn::derivation(exp, l, breaking);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct derived_term: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    expression exp = read_expression(opts);
    std::string algo = 1 <= opts.argv.size() ? opts.argv[0] : "auto";

    // Process.
    automaton res = vcsn::dyn::derived_term(exp, algo);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct determinize: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);

    // Process.
    auto res = vcsn::dyn::determinize(aut);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct eliminate_state: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    int n = (0 < opts.argv.size()
             ? boost::lexical_cast<int>(opts.argv[0])
             : -1);

    // Process.
    auto res = vcsn::dyn::eliminate_state(aut, n);

    // Output.
    opts.print(res);
    return 0;
  }
};


struct evaluate: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    vcsn::require(0 < opts.argv.size(),
                  "vcsn evaluate: too few arguments: ",
                  opts.argv.size(), ", expected 2");
    options opts2 = opts;
    opts2.input = opts.argv[0];
    opts2.input_is_file = false;
    vcsn::dyn::label l = read_word(opts2);

    // Process.
    auto res = vcsn::dyn::eval(aut, l);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct lweight: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton aut = read_automaton(opts);
    vcsn::require(1 < opts.argv.size(),
                  "vcsn lweight: too few arguments: ",
                  opts.argv.size(), ", expected 2");

    // Hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    opts2.input_is_file = false;
    weight w = read_weight(opts2);

    // Process.
    auto res = vcsn::dyn::lweight(w, aut);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct minimize: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    std::string algo = 1 <= opts.argv.size() ? opts.argv[0] : "signature";

    // Process.
    auto res = vcsn::dyn::minimize(aut, algo);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct rweight: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton aut = read_automaton(opts);
    vcsn::require(1 < opts.argv.size(),
                  "vcsn rweight: too few arguments: ",
                  opts.argv.size(), ", expected 2");

    // Hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    opts2.input_is_file = false;
    weight w = read_weight(opts2);

    // Process.
    auto res = vcsn::dyn::rweight(aut, w);

    // Output.
    opts.print(res);
    return 0;
  }
};

struct shortest: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    if (opts.output_format == "default"
        || opts.output_format == "")
      vcsn::dyn::set_format(*opts.out, "list");

    auto aut = read_automaton(opts);
    unsigned num = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);
    unsigned len = (1 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[1])
                    : -1U);

    auto res = vcsn::dyn::shortest(aut, num, len);

    opts.print(res);
    return 0;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    if (opts.output_format == "default"
        || opts.output_format == "")
      vcsn::dyn::set_format(*opts.out, "list");

    auto exp = read_expression(opts);
    unsigned num = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);
    unsigned len = (1 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[1])
                    : -1U);

    auto res = vcsn::dyn::shortest(vcsn::dyn::standard(exp), num, len);

    opts.print(res);
    return 0;
  }
};


/*-------------------.
| "Main" functions.  |
`-------------------*/

static int de_bruijn(int argc, char * const argv[])
{
  options opts;
  opts.input_format = "text";
  parse_args(opts, argc, argv);

  // Input.
  using namespace vcsn::dyn;
  auto ctx = vcsn::dyn::make_context(opts.context);
  vcsn::require(argc == 1,
                "vcsn de_bruijn: invalid number of arguments: ",
                argc, ", expected 1");
  size_t n = boost::lexical_cast<size_t>(argv[0]);

  // Process.
  automaton aut = de_bruijn(ctx, n);

  // Output.
  opts.print(aut);
  return 0;
}

static int double_ring(int argc, char * const argv[])
{
  options opts;
  opts.input_format = "text";
  parse_args(opts, argc, argv);

  // Input.
  using namespace vcsn::dyn;
  auto ctx = vcsn::dyn::make_context(opts.context);
  vcsn::require(1 < argc,
                "vcsn double_ring: too few arguments: ",
                argc, ", expected at least 1");
  size_t n = boost::lexical_cast<size_t>(argv[0]);
  // final states.
  std::vector<unsigned> f;
  for (int i = 1; i < argc; ++i)
    f.emplace_back(boost::lexical_cast<unsigned>(argv[i]));

  // Process.
  automaton aut = double_ring(ctx, n, f);

  // Output.
  opts.print(aut);
  return 0;
}

static int ladybird(int argc, char * const argv[])
{
  options opts;
  opts.input_format = "text";
  parse_args(opts, argc, argv);

  // Input.
  using namespace vcsn::dyn;
  auto ctx = vcsn::dyn::make_context(opts.context);
  vcsn::require(argc == 1,
                "vcsn ladybird: invalid number of arguments: ",
                argc, ", expected 1");
  size_t n = boost::lexical_cast<size_t>(argv[0]);

  // Process.
  automaton aut = ladybird(ctx, n);

  // Output.
  opts.print(aut);
  return 0;
}

static int random(int argc, char * const argv[])
{
  options opts;
  parse_args(opts, argc, argv);

  // Input.
  using namespace vcsn::dyn;
  auto ctx = vcsn::dyn::make_context(opts.context);
  vcsn::require(0 < argc && argc <= 4,
                "vcsn random: invalid number of arguments: ",
                argc, ", expected from 1 to 4");
  unsigned num_states  =            boost::lexical_cast<unsigned>(argv[0]);
  float density        = 1 < argc ? boost::lexical_cast<float>(argv[1])    : .1;
  unsigned num_initial = 2 < argc ? boost::lexical_cast<unsigned>(argv[2]) : 1;
  unsigned num_final   = 3 < argc ? boost::lexical_cast<unsigned>(argv[3]) : 1;

  // Process.
  automaton aut = random_automaton(ctx, num_states, density,
                                   num_initial, num_final);

  // Output.
  opts.print(aut);
  return 0;
}

static int u(int argc, char * const argv[])
{
  options opts;
  opts.input_format = "text";
  parse_args(opts, argc, argv);

  // Input.
  using namespace vcsn::dyn;
  auto ctx = vcsn::dyn::make_context(opts.context);
  vcsn::require(argc == 1,
                "vcsn u: invalid number of arguments: ",
                argc, ", expected 1");
  auto n = boost::lexical_cast<unsigned>(argv[0]);

  // Process.
  automaton aut = u(ctx, n);

  // Output.
  opts.print(aut);
  return 0;
}


/*-------.
| Main.  |
`-------*/

int main(int argc, char* const argv[])
try
{
  auto opts = parse_args(argc, argv);

  if (opts.cmd == "help")        usage(argv[0], EXIT_SUCCESS);
  if (opts.cmd == "version")     version(opts);
  if (opts.cmd == "de_bruijn")   return de_bruijn(argc - 1, argv + 1);
  if (opts.cmd == "divkbaseb")   return divkbaseb(argc - 1, argv + 1);
  if (opts.cmd == "double_ring") return double_ring(argc - 1, argv + 1);
  if (opts.cmd == "ladybird")    return ladybird(argc - 1, argv + 1);
  if (opts.cmd == "quotkbaseb")  return quotkbaseb(argc - 1, argv + 1);
  if (opts.cmd == "random")      return random(argc - 1, argv + 1);
  if (opts.cmd == "u")           return u(argc - 1, argv + 1);
  else
    {
      std::unique_ptr<vcsn_function> f;
#define ALGO(Name)                                                      \
      else if (opts.cmd == #Name) f = std::unique_ptr<Name>(new Name{})
      if (false) {}
      ALGO(accessible);
      ALGO(add);
      ALGO(are_equivalent);
      ALGO(are_isomorphic);
      ALGO(cat);
      ALGO(coaccessible);
      ALGO(complement);
      ALGO(complete);
      ALGO(compose);
      ALGO(conjunction);
      ALGO(constant_term);
      ALGO(derivation);
      ALGO(derived_term);
      ALGO(determinize);
      ALGO(difference);
      ALGO(eliminate_state);
      ALGO(evaluate);
      ALGO(expand);
      ALGO(inductive);
      ALGO(infiltrate);
      ALGO(is_ambiguous);
      ALGO(is_complete);
      ALGO(is_deterministic);
      ALGO(is_empty);
      ALGO(is_eps_acyclic);
      ALGO(is_normalized);
      ALGO(is_proper);
      ALGO(is_standard);
      ALGO(is_trim);
      ALGO(is_useless);
      ALGO(is_valid);
      ALGO(lift);
      ALGO(minimize);
      ALGO(multiply);
      ALGO(proper);
      ALGO(shortest);
      ALGO(shuffle);
      ALGO(split);
      ALGO(standard);
      ALGO(star);
      ALGO(star_normal_form);
      ALGO(thompson);
      ALGO(to_expression);
      ALGO(transpose);
      ALGO(trim);
      ALGO(universal);
      vcsn::require(f != nullptr, "unknown command: ", opts.cmd);
      switch (opts.input_type)
        {
        case type::automaton:  return f->work_aut(opts);
        case type::polynomial: return f->work_polynomial(opts);
        case type::expression: return f->work_exp(opts);
        case type::weight:     return f->work_weight(opts);
        }
    }
}
catch (const std::exception& e)
{
  std::cerr << "error: " << e.what() << '\n';
  exit(EXIT_FAILURE);
}
catch (...)
{
  std::cerr << "error: unknown exception caught\n";
  exit(EXIT_FAILURE);
}
