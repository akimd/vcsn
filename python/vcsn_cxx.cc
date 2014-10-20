#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic ignored "-Wmissing-declarations"

// python/vcsn_cxx.cc: In constructor 'minimize::minimize(const boost::python::detail::keywords<nkeywords>&, const char*)':
// python/vcsn_cxx.cc:445:1531: warning: typedef 'assertion' locally defined but not used [-Wunused-local-typedefs]
// BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(minimize, minimize, 0, 1);
# pragma GCC diagnostic ignored "-Wunused-local-typedefs"

# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <boost/python.hpp>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh> // vname
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>

struct automaton;
struct context;
struct expansion;
struct label;
struct polynomial;
struct ratexp;
struct weight;

/// Convert a Python list to a C++ vector.
template <typename T>
std::vector<T>
to_vector(const boost::python::list& list)
{
  std::vector<T> res;
  for (int i = 0; i < boost::python::len(list); ++i)
    res.emplace_back(boost::python::extract<T>(list[i]));
  return res;
}

/*----------.
| context.  |
`----------*/

struct context
{
  context(const vcsn::dyn::context& ctx)
    : val_(ctx)
  {}

  context(const std::string& ctx)
    : context(vcsn::dyn::make_context(ctx))
  {}

  automaton cerny(unsigned n) const;

  automaton de_bruijn(unsigned n) const;

  automaton divkbaseb(unsigned divisor, unsigned base) const;

  automaton double_ring(unsigned n, const boost::python::list& finals) const;

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  automaton ladybird(unsigned n) const;

  automaton random(unsigned num_states, float density = 0.1,
                   unsigned num_initial = 1, unsigned num_final = 1) const;
  automaton random_deterministic(unsigned num_states) const;

  ratexp series(const std::string& s) const;

  automaton u(unsigned num_states) const;

  label word(const std::string& s) const;

  vcsn::dyn::context val_;
};

/*------------.
| automaton.  |
`------------*/

struct automaton
{
  automaton(const vcsn::dyn::automaton& a)
    : val_(a)
  {}

  automaton(const ratexp& r);

  automaton(const std::string& s, const std::string& format = "default")
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_automaton(is, format);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  automaton accessible() const
  {
    return vcsn::dyn::accessible(val_);
  }

  label ambiguous_word() const;

  automaton blind(unsigned tape)
  {
    return vcsn::dyn::blind(val_, tape);
  }

  automaton chain(int min, int max) const
  {
    return vcsn::dyn::chain(val_, min, max);
  }
  /// The type of the previous function.
  using bin_chain_t = automaton (automaton::*)(int min, int max) const;

  automaton chain(int min) const
  {
    return chain(min, min);
  }

  automaton coaccessible() const
  {
    return vcsn::dyn::coaccessible(val_);
  }

  automaton codeterminize(const std::string& algo = "auto") const
  {
    return vcsn::dyn::codeterminize(val_, algo);
  }

  automaton cominimize(const std::string& algo = "weighted") const
  {
    return vcsn::dyn::cominimize(val_, algo);
  }

  automaton complement() const
  {
    return vcsn::dyn::complement(val_);
  }

  automaton complete() const
  {
    return vcsn::dyn::complete(val_);
  }

  automaton compose(automaton& rhs)
  {
    return vcsn::dyn::compose(val_, rhs.val_);
  }

  automaton concatenate(const automaton& rhs) const
  {
    return vcsn::dyn::concatenate(val_, rhs.val_);
  }

  ::context context() const
  {
    return vcsn::dyn::context_of(val_);
  }

  automaton costandard() const
  {
    return vcsn::dyn::costandard(val_);
  }

  automaton determinize(const std::string& algo = "auto") const
  {
    return vcsn::dyn::determinize(val_, algo);
  }

  automaton difference(const automaton& rhs) const
  {
    return vcsn::dyn::difference(val_, rhs.val_);
  }

  automaton eliminate_state(int s) const
  {
    return vcsn::dyn::eliminate_state(val_, s);
  }

  polynomial enumerate(unsigned max) const;

  weight eval(const label& l) const;

  automaton factor() const
  {
    return vcsn::dyn::factor(val_);
  }

  std::string format(const std::string& format = "dot") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  bool has_twins_property() const
  {
    return vcsn::dyn::has_twins_property(val_);
  }

  static automaton infiltration_(const boost::python::list& auts)
  {
    return vcsn::dyn::infiltration(automata_(auts));
  }

  automaton insplit() const
  {
    return vcsn::dyn::insplit(val_);
  }

  bool is_accessible() const
  {
    return vcsn::dyn::is_accessible(val_);
  }

  bool is_ambiguous() const
  {
    return vcsn::dyn::is_ambiguous(val_);
  }

  bool is_coaccessible() const
  {
    return vcsn::dyn::is_coaccessible(val_);
  }

  bool is_codeterministic() const
  {
    return vcsn::dyn::is_codeterministic(val_);
  }

  bool is_complete() const
  {
    return vcsn::dyn::is_complete(val_);
  }

  bool is_costandard() const
  {
    return vcsn::dyn::is_costandard(val_);
  }

  bool is_cycle_ambiguous() const
  {
    return vcsn::dyn::is_cycle_ambiguous(val_);
  }

  bool is_deterministic() const
  {
    return vcsn::dyn::is_deterministic(val_);
  }

  bool is_empty() const
  {
    return vcsn::dyn::is_empty(val_);
  }

  bool is_eps_acyclic() const
  {
    return vcsn::dyn::is_eps_acyclic(val_);
  }

  bool is_equivalent(const automaton& rhs) const
  {
    return vcsn::dyn::are_equivalent(val_, rhs.val_);
  }

  bool is_functional() const
  {
    return vcsn::dyn::is_functional(val_);
  }

  bool is_isomorphic(const automaton& rhs) const
  {
    return vcsn::dyn::are_isomorphic(val_, rhs.val_);
  }

  bool is_normalized() const
  {
    return vcsn::dyn::is_normalized(val_);
  }

  bool is_out_sorted() const
  {
    return vcsn::dyn::is_out_sorted(val_);
  }

  bool is_proper() const
  {
    return vcsn::dyn::is_proper(val_);
  }

  bool is_standard() const
  {
    return vcsn::dyn::is_standard(val_);
  }

  bool is_synchronized_by(const label& word) const;

  bool is_synchronizing() const
  {
    return vcsn::dyn::is_synchronizing(val_);
  }

  bool is_trim() const
  {
    return vcsn::dyn::is_trim(val_);
  }

  bool is_useless() const
  {
    return vcsn::dyn::is_useless(val_);
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(val_);
  }

  automaton left_mult(const weight& w) const;

  automaton lift() const
  {
    return vcsn::dyn::lift(val_);
  }

  automaton minimize(const std::string& algo = "weighted") const
  {
    return vcsn::dyn::minimize(val_, algo);
  }

  automaton normalize() const
  {
    return vcsn::dyn::normalize(val_);
  }

  std::size_t num_sccs() const
  {
    return vcsn::dyn::num_sccs(val_);
  }

  automaton pair(bool keep_initials = false) const
  {
    return vcsn::dyn::pair(val_, keep_initials);
  }

  automaton prefix() const
  {
    return vcsn::dyn::prefix(val_);
  }

  automaton power(unsigned n) const
  {
    return vcsn::dyn::power(val_, n);
  }

  static automaton product_(const boost::python::list& auts)
  {
    return vcsn::dyn::product(automata_(auts));
  }

  automaton proper(bool prune = true, bool backward = true) const
  {
    return vcsn::dyn::proper(val_,
                             backward
                             ? vcsn::direction::backward
                             : vcsn::direction::forward,
                             prune);
  }

  automaton push_weights() const
  {
    return vcsn::dyn::push_weights(val_);
  }

  automaton reduce() const
  {
    return vcsn::dyn::reduce(val_);
  }

  automaton right_mult(const weight& w) const;

  polynomial shortest(unsigned max) const;

  static automaton shuffle_(const boost::python::list& auts)
  {
    return vcsn::dyn::shuffle(automata_(auts));
  }

  automaton sort() const
  {
    return vcsn::dyn::sort(val_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(val_);
  }

  automaton star() const
  {
    return vcsn::dyn::star(val_);
  }

  automaton strip() const
  {
    return vcsn::dyn::strip(val_);
  }

  automaton suffix() const
  {
    return vcsn::dyn::suffix(val_);
  }

  automaton subword() const
  {
    return vcsn::dyn::subword(val_);
  }

  automaton sum(const automaton& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  label synchronizing_word(const std::string& algo = "greedy") const;

  ratexp to_ratexp(const std::string& algo = "auto") const;

  automaton transpose()
  {
    return vcsn::dyn::transpose(val_);
  }

  automaton trim() const
  {
    return vcsn::dyn::trim(val_);
  }

  automaton union_a(const automaton& rhs) const
  {
    return vcsn::dyn::union_a(val_, rhs.val_);
  }

  automaton universal() const
  {
    return vcsn::dyn::universal(val_);
  }

  /// Convert to a vector of automata.
  using automata_t = std::vector<vcsn::dyn::automaton>;
  static automata_t automata_(const boost::python::list& auts)
  {
    automata_t res;
    for (int i = 0; i < boost::python::len(auts); ++i)
      res.emplace_back(boost::python::extract<automaton>(auts[i])().val_);
    return res;
  }

  vcsn::dyn::automaton val_;
};

/*------------.
| expansion.  |
`------------*/

struct expansion
{
  expansion(const vcsn::dyn::expansion& val)
    : val_(val)
  {}

  expansion(const std::string&)
  {
    vcsn::raise("not implemented");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  vcsn::dyn::expansion val_;
};

/*--------.
| label.  |
`--------*/

struct label
{
  label(const vcsn::dyn::label& val)
    : val_(val)
  {}

  label(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_label(is, ctx.val_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  vcsn::dyn::label val_;
};

/*-------------.
| polynomial.  |
`-------------*/

struct polynomial
{
  polynomial(const vcsn::dyn::polynomial& val)
    : val_(val)
  {}

  polynomial(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_polynomial(is, ctx.val_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  polynomial concatenate(const polynomial& rhs) const
  {
    return vcsn::dyn::concatenate(val_, rhs.val_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  polynomial split() const
  {
    return vcsn::dyn::split(val_);
  }

  polynomial sum(const polynomial& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  vcsn::dyn::polynomial val_;
};

/*---------.
| ratexp.  |
`---------*/

struct ratexp
{
  ratexp(const vcsn::dyn::ratexp& r)
    : val_(r)
  {}

  ratexp(const context& ctx, const std::string& r,
         vcsn::rat::identities i)
  {
    std::istringstream is(r);
    auto rs = vcsn::dyn::make_ratexpset(ctx.val_, i);
    val_ = vcsn::dyn::read_ratexp(is, rs);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  ratexp(const context& ctx, const std::string& r)
    : ratexp(ctx, r, vcsn::rat::identities::trivial)
  {}

  static ratexp series(const context& ctx, const std::string& r)
  {
    return ratexp(ctx, r, vcsn::rat::identities::series);
  }

  ratexp chain(int min, int max) const
  {
    return vcsn::dyn::chain(val_, min, max);
  }
  /// The type of the previous function.
  using bin_chain_t = ratexp (ratexp::*)(int min, int max) const;

  ratexp chain(int min) const
  {
    return chain(min, min);
  }

  ratexp complement() const
  {
    return vcsn::dyn::complement(val_);
  }

  ratexp concatenate(const ratexp& rhs) const
  {
    return vcsn::dyn::concatenate(val_, rhs.val_);
  }

  ratexp conjunction(const ratexp& rhs) const
  {
    return vcsn::dyn::conjunction(val_, rhs.val_);
  }

  weight constant_term() const;

  ::context context() const
  {
    return vcsn::dyn::context_of(val_);
  }

  polynomial derivation(const label& l, bool breaking = false) const
  {
    return vcsn::dyn::derivation(val_, l.val_, breaking);
  }

  ratexp as_(const ::context& ctx, vcsn::rat::identities ids)
  {
    auto rs = vcsn::dyn::make_ratexpset(ctx.val_, ids);
    return vcsn::dyn::copy(val_, rs);
  }

  ratexp as_ratexp_in(const ::context ctx)
  {
    return as_(ctx, vcsn::rat::identities::trivial);
  }
  ratexp as_ratexp()
  {
    return as_(::context(vcsn::dyn::context_of(val_)),
                 vcsn::rat::identities::trivial);
  }
  ratexp as_series_in(const ::context ctx)
  {
    return as_(ctx, vcsn::rat::identities::series);
  }
  ratexp as_series()
  {
    return as_(::context(vcsn::dyn::context_of(val_)),
                 vcsn::rat::identities::series);
  }

  automaton derived_term(const std::string& algo = "auto") const
  {
    return vcsn::dyn::derived_term(val_, algo);
  }

  ratexp difference(const ratexp& rhs) const
  {
    return vcsn::dyn::difference(val_, rhs.val_);
  }

  ratexp expand() const
  {
    return vcsn::dyn::expand(val_);
  }

  expansion to_expansion() const
  {
    return vcsn::dyn::to_expansion(val_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  bool is_equivalent(const ratexp& rhs) const
  {
    return vcsn::dyn::are_equivalent(val_, rhs.val_);
  }

  bool is_series() const
  {
    return vcsn::dyn::identities(val_) == vcsn::rat::identities::series;
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(val_);
  }

  ratexp left_mult(const weight& w) const;

  ratexp lift() const
  {
    return vcsn::dyn::lift(val_);
  }

  ratexp right_mult(const weight& w) const;

  ratexp shuffle(const ratexp& rhs)
  {
    return vcsn::dyn::shuffle(val_, rhs.val_);
  }

  polynomial split() const
  {
    return vcsn::dyn::split(val_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(val_);
  }

  unsigned star_height() const
  {
    return vcsn::dyn::star_height(val_);
  }

  ratexp star_normal_form() const
  {
    return vcsn::dyn::star_normal_form(val_);
  }

  ratexp sum(const ratexp& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  automaton thompson() const
  {
    return vcsn::dyn::thompson(val_);
  }

  ratexp transpose() const
  {
    return vcsn::dyn::transpose(val_);
  }

  ratexp transposition() const
  {
    return vcsn::dyn::transposition(val_);
  }

  vcsn::dyn::ratexp val_;
};

ratexp context::series(const std::string& s) const
{
  return ratexp::series(*this, s);
}

/*---------.
| weight.  |
`---------*/

struct weight
{
  weight(const vcsn::dyn::weight& val)
    : val_(val)
  {}

  weight(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_weight(is, ctx.val_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  weight multiply(const weight& rhs) const
  {
    return vcsn::dyn::multiply(val_, rhs.val_);
  }

  weight sum(const weight& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  vcsn::dyn::weight val_;
};

/*---------------------------.
| automaton implementation.  |
`---------------------------*/

automaton::automaton(const ratexp& r)
{
  *this = r.derived_term();
}

label automaton::ambiguous_word() const
{
  return vcsn::dyn::ambiguous_word(val_);
}

polynomial automaton::enumerate(unsigned max) const
{
  return vcsn::dyn::enumerate(val_, max);
}

weight automaton::eval(const label& l) const
{
  return vcsn::dyn::eval(val_, l.val_);
}

bool automaton::is_synchronized_by(const label& word) const
{
  return vcsn::dyn::is_synchronized_by(val_, word.val_);
}

automaton automaton::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, val_);
}

automaton automaton::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(val_, w.val_);
}

polynomial automaton::shortest(unsigned max) const
{
  return vcsn::dyn::shortest(val_, max);
}

label automaton::synchronizing_word(const std::string& algo) const
{
  return vcsn::dyn::synchronizing_word(val_, algo);
}

ratexp automaton::to_ratexp(const std::string& algo) const
{
  return vcsn::dyn::to_expression(val_, algo);
}

/*-------------------------.
| context implementation.  |
`-------------------------*/

automaton context::cerny(unsigned n) const
{
  return vcsn::dyn::cerny(val_, n);
}

automaton context::de_bruijn(unsigned n) const
{
  return vcsn::dyn::de_bruijn(val_, n);
}

automaton context::divkbaseb(unsigned divisor, unsigned base) const
{
  return vcsn::dyn::divkbaseb(val_, divisor, base);
}

automaton context::double_ring(unsigned n, const boost::python::list& finals) const
{
  return vcsn::dyn::double_ring(val_, n, to_vector<unsigned>(finals));
}

automaton context::ladybird(unsigned n) const
{
  return vcsn::dyn::ladybird(val_, n);
}

automaton context::random(unsigned num_states, float density,
                          unsigned num_initial, unsigned num_final) const
{
  return vcsn::dyn::random_automaton(val_,
                                     num_states, density,
                                     num_initial, num_final);
}

automaton context::random_deterministic(unsigned num_states) const
{
  return vcsn::dyn::random_automaton_deterministic(val_, num_states);
}

automaton context::u(unsigned num_states) const
{
  return vcsn::dyn::u(val_, num_states);
}

label context::word(const std::string& s) const
{
  return label(context(vcsn::dyn::make_word_context(val_)), s);
}


/*------------------------.
| ratexp implementation.  |
`------------------------*/

weight ratexp::constant_term() const
{
  return vcsn::dyn::constant_term(val_);
}

ratexp ratexp::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, val_);
}

ratexp ratexp::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(val_, w.val_);
}


/*-----------.
| vcsn_cxx.  |
`-----------*/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(chain, chain, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(derivation, derivation, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(derived_term, derived_term, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(codeterminize, codeterminize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(cominimize, cominimize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(determinize, determinize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(minimize, minimize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pair, pair, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(random_overloads, random, 1, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(synchronizing_word, synchronizing_word,
                                       0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(to_ratexp, to_ratexp, 0, 1);

BOOST_PYTHON_MODULE(vcsn_cxx)
{
  namespace bp = boost::python;
  using bp::arg;

  bp::class_<automaton>
    ("automaton",
     bp::init<const ratexp&>())
    .def(bp::init<const std::string&, bp::optional<const std::string&>>())

    .def("accessible", &automaton::accessible)
    .def("ambiguous_word", &automaton::ambiguous_word)
    .def("blind", &automaton::blind)
    .def("chain", static_cast<automaton::bin_chain_t>(&automaton::chain), chain())
    .def("coaccessible", &automaton::coaccessible)
    .def("codeterminize", &automaton::codeterminize, codeterminize())
    .def("cominimize", &automaton::cominimize, cominimize())
    .def("complement", &automaton::complement)
    .def("complete", &automaton::complete)
    .def("compose", &automaton::compose)
    .def("concatenate", &automaton::concatenate)
    .def("context", &automaton::context)
    .def("costandard", &automaton::costandard)
    .def("determinize", &automaton::determinize, determinize())
    .def("difference", &automaton::difference)
    .def("eliminate_state", &automaton::eliminate_state)
    .def("enumerate", &automaton::enumerate)
    .def("_eval", &automaton::eval)
    .def("factor", &automaton::factor)
    .def("_format", &automaton::format)
    .def("has_twins_property", &automaton::has_twins_property)
    .def("_infiltration", &automaton::infiltration_).staticmethod("_infiltration")
    .def("insplit", &automaton::insplit)
    .def("is_accessible", &automaton::is_accessible)
    .def("is_ambiguous", &automaton::is_ambiguous)
    .def("is_coaccessible", &automaton::is_coaccessible)
    .def("is_codeterministic", &automaton::is_codeterministic)
    .def("is_complete", &automaton::is_complete)
    .def("is_costandard", &automaton::is_costandard)
    .def("is_cycle_ambiguous", &automaton::is_cycle_ambiguous)
    .def("is_deterministic", &automaton::is_deterministic)
    .def("is_empty", &automaton::is_empty)
    .def("is_eps_acyclic", &automaton::is_eps_acyclic)
    .def("is_equivalent", &automaton::is_equivalent)
    .def("is_functional", &automaton::is_functional)
    .def("is_isomorphic", &automaton::is_isomorphic)
    .def("is_normalized", &automaton::is_normalized)
    .def("is_proper", &automaton::is_proper)
    .def("is_out_sorted", &automaton::is_out_sorted)
    .def("is_standard", &automaton::is_standard)
    .def("_is_synchronized_by", &automaton::is_synchronized_by)
    .def("is_synchronizing", &automaton::is_synchronizing)
    .def("is_trim", &automaton::is_trim)
    .def("is_useless", &automaton::is_useless)
    .def("is_valid", &automaton::is_valid)
    .def("left_mult", &automaton::left_mult)
    .def("lift", &automaton::lift)
    .def("minimize", &automaton::minimize, minimize())
    .def("normalize", &automaton::normalize)
    .def("num_sccs", &automaton::num_sccs)
    .def("pair", &automaton::pair, pair())
    .def("prefix", &automaton::prefix)
    .def("power", &automaton::power)
    .def("_product", &automaton::product_).staticmethod("_product")
    .def("_proper", &automaton::proper,
         (arg("prune") = true, arg("backward") = true))
    .def("push_weights", &automaton::push_weights)
    .def("ratexp", &automaton::to_ratexp, to_ratexp())
    .def("reduce", &automaton::reduce)
    .def("right_mult", &automaton::right_mult)
    .def("shortest", &automaton::shortest)
    .def("_shuffle", &automaton::shuffle_).staticmethod("_shuffle")
    .def("sort", &automaton::sort)
    .def("standard", &automaton::standard)
    .def("star", &automaton::star)
    .def("strip", &automaton::strip)
    .def("suffix", &automaton::suffix)
    .def("subword", &automaton::subword)
    .def("sum", &automaton::sum)
    .def("synchronizing_word", &automaton::synchronizing_word,
                               synchronizing_word())
    .def("transpose", &automaton::transpose)
    .def("trim", &automaton::trim)
    .def("union", &automaton::union_a)
    .def("universal", &automaton::universal)
    ;

  bp::class_<context>
    ("context",
     bp::init<const std::string&>())
    .def("cerny", &context::cerny)
    .def("de_bruijn", &context::de_bruijn)
    .def("divkbaseb", &context::divkbaseb)
    .def("double_ring", &context::double_ring)
    .def("format", &context::format)
    .def("ladybird", &context::ladybird)
    .def("random", &context::random, random_overloads())
    .def("random_deterministic", &context::random_deterministic)
    .def("series", &context::series)
    .def("u", &context::u)
    .def("word", &context::word)
   ;

  bp::class_<expansion>
    ("expansion",
     bp::init<const std::string&>())
    .def("format", &expansion::format)
   ;

  bp::class_<label>
    ("label",
     bp::init<const context&, const std::string&>())
    .def("format", &label::format)
   ;

  bp::class_<polynomial>
    ("polynomial",
     bp::init<const context&, const std::string&>())
    .def("concatenate", &polynomial::concatenate)
    .def("format", &polynomial::format)
    .def("split", &polynomial::split)
    .def("sum", &polynomial::sum)
   ;

  bp::class_<ratexp>
    ("ratexp",
     bp::init<const context&, const std::string&>())
    .def("chain", static_cast<ratexp::bin_chain_t>(&ratexp::chain), chain())
    .def("complement", &ratexp::complement)
    .def("concatenate", &ratexp::concatenate)
    .def("conjunction", &ratexp::conjunction)
    .def("constant_term", &ratexp::constant_term)
    .def("context", &ratexp::context)
    .def("_as_ratexp_in", &ratexp::as_ratexp_in)
    .def("_as_series_in", &ratexp::as_series_in)
    .def("_derivation", &ratexp::derivation, derivation())
    .def("derived_term", &ratexp::derived_term, derived_term())
    .def("difference", &ratexp::difference)
    .def("expand", &ratexp::expand)
    .def("expansion", &ratexp::to_expansion)
    .def("format", &ratexp::format)
    .def("is_equivalent", &ratexp::is_equivalent)
    .def("is_series", &ratexp::is_series)
    .def("is_valid", &ratexp::is_valid)
    .def("left_mult", &ratexp::left_mult)
    .def("lift", &ratexp::lift)
    .def("ratexp", &ratexp::as_ratexp)
    .def("right_mult", &ratexp::right_mult)
    .def("series", &ratexp::as_series)
    .def("shuffle", &ratexp::shuffle)
    .def("split", &ratexp::split)
    .def("standard", &ratexp::standard)
    .def("star_height", &ratexp::star_height)
    .def("star_normal_form", &ratexp::star_normal_form)
    .def("sum", &ratexp::sum)
    .def("thompson", &ratexp::thompson)
    .def("transpose", &ratexp::transpose)
    .def("transposition", &ratexp::transposition)
    ;

  bp::class_<weight>
    ("weight",
     bp::init<const context&, const std::string&>())
    .def("format", &weight::format)
    .def("multiply", &weight::multiply)
    .def("sum", &weight::sum)
   ;

}
