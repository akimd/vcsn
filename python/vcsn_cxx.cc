#if defined __clang__
# if 3 <= __clang_major__ && 6 <= __clang_minor__
// python/vcsn_cxx.cc: In constructor
//   'minimize::minimize(const boost::python::detail::keywords<nkeywords>&,
//                       const char*)':
// python/vcsn_cxx.cc:445:1531: warning: typedef 'assertion'
//   locally defined but not used [-Wunused-local-typedefs]
//
// BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(minimize, minimize, 0, 1);
# pragma GCC diagnostic ignored "-Wunused-local-typedefs"
# endif
#elif defined __GNUC__
# pragma GCC diagnostic ignored "-Wmissing-declarations"
# pragma GCC diagnostic ignored "-Wunused-local-typedefs"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <boost/python.hpp>
#include <boost/python/args.hpp>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh> // vname
#include <vcsn/dyn/expansion.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>

/// See http://stackoverflow.com/a/6794523/1353549.
///
/// Invoke `python_optional<type_t>()` from the module initialization
/// for all the needed `type_t`s.
template <typename T>
struct python_optional
  : private boost::noncopyable
{
  struct conversion
    : public boost::python::converter::expected_from_python_type<T>
  {
    static PyObject* convert(boost::optional<T> const& value)
    {
      using namespace boost::python;
      return incref((value ? object(*value) : object()).ptr());
    }
  };

  static void* convertible(PyObject* obj)
  {
    using namespace boost::python;
    return obj == Py_None || extract<T>(obj).check() ? obj : nullptr;
  }

  static void
  constructor(PyObject *obj,
              boost::python::converter::rvalue_from_python_stage1_data *data)
  {
    using namespace boost::python;
    using data_t = converter::rvalue_from_python_storage<boost::optional<T>>;
    void *const storage =reinterpret_cast<data_t*>(data)->storage.bytes;
    if (obj == Py_None)
      new (storage) boost::optional<T>();
    else
      new (storage) boost::optional<T>(extract<T>(obj));
    data->convertible = storage;
  }

  explicit python_optional()
  {
    using namespace boost::python;
    if (!extract<boost::optional<T>>(object()).check())
      {
        to_python_converter<boost::optional<T>, conversion, true>();
        converter::registry::push_back(&convertible,
                                       &constructor,
                                       type_id<boost::optional<T> >(),
                                       &conversion::get_pytype);
      }
  }
};

/// Convert to identities.
auto identities(const std::string& s)
  -> vcsn::rat::identities
{
  std::istringstream is{s};
  vcsn::rat::identities res;
  is >> res;
  vcsn::require(is.peek() == EOF, "unexpected trailing characters: ", is);
  return res;
}

/// Convert a Python list to a C++ vector.
template <typename T>
auto make_vector(const boost::python::list& list)
  -> std::vector<T>
{
  std::vector<T> res;
  for (int i = 0; i < boost::python::len(list); ++i)
    res.emplace_back(boost::python::extract<T>(list[i]));
  return res;
}

/// Create an input stream from a file, or from a string.
auto make_istream(const std::string& data = "",
                  const std::string& filename = "")
  -> std::shared_ptr<std::istream>
{
  vcsn::require(!data.empty() || !filename.empty(),
                "cannot provide both data and filename");
  if (!data.empty())
    return std::make_shared<std::istringstream>(data);
  else if (!filename.empty())
    return vcsn::open_input_file(filename);
  else
    vcsn::raise("must provide either data or filename");
}



struct automaton;
struct context;
struct expansion;
struct label;
struct polynomial;
struct expression;
struct weight;


/*----------.
| context.  |
`----------*/

struct context
{
  context()
  {}

  context(const vcsn::dyn::context& ctx)
    : val_(ctx)
  {}

  context(const std::string& ctx)
    : context(vcsn::dyn::make_context(ctx))
  {}

  explicit operator bool() const
  {
    return bool(val_);
  }

  automaton cerny(unsigned n) const;

  automaton cotrie(const std::string& data = "",
                   const std::string& format = "default",
                   const std::string& filename = "") const;

  automaton de_bruijn(unsigned n) const;

  automaton divkbaseb(unsigned divisor, unsigned base) const;

  automaton double_ring(unsigned n, const boost::python::list& finals) const;

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  context join(const context& rhs) const
  {
    return vcsn::dyn::join(val_, rhs.val_);
  }

  automaton ladybird(unsigned n) const;

  automaton levenshtein() const;

  context project(unsigned tape) const;

  automaton random(unsigned num_states, float density = 0.1,
                   unsigned num_initial = 1, unsigned num_final = 1,
                   boost::optional<unsigned> max_labels = {},
                   float loop_chance = 0.0) const;
  automaton random_deterministic(unsigned num_states) const;

  expression series(const std::string& s) const;

  automaton trie(const std::string& data = "",
                 const std::string& format = "default",
                 const std::string& filename = "") const;

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

  /// Derived-term automaton from r.
  automaton(const expression& r);

  /// Create an automaton from a file, or from a string.
  automaton(const std::string& data = "",
            const std::string& format = "default",
            const std::string& filename = "",
            bool strip = true)
    : val_(nullptr)
  {
    auto is = make_istream(data, filename);
    val_ = vcsn::dyn::read_automaton(*is, format, strip);
    vcsn::require(is->peek() == EOF, "unexpected trailing characters: ", *is);
  }

  automaton accessible() const
  {
    return vcsn::dyn::accessible(val_);
  }

  label ambiguous_word() const;

  /// Convert \a this to \a ctx, using \a ids.
  automaton as(const ::context& ctx) const
  {
    return vcsn::dyn::copy(val_, ctx.val_);
  }

  automaton coaccessible() const
  {
    return vcsn::dyn::coaccessible(val_);
  }

  automaton codeterminize(const std::string& algo = "auto") const
  {
    return vcsn::dyn::codeterminize(val_, algo);
  }

  automaton cominimize(const std::string& algo = "auto") const
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

  automaton component(unsigned com_num) const
  {
    return vcsn::dyn::component(val_, com_num);
  }

  automaton compose(automaton& rhs, bool lazy = false)
  {
    return vcsn::dyn::compose(val_, rhs.val_, lazy);
  }

  automaton condense() const
  {
    return vcsn::dyn::condense(val_);
  }

  automaton conjugate()
  {
    return vcsn::dyn::conjugate(val_);
  }

  automaton conjunction(unsigned n) const
  {
    return vcsn::dyn::conjunction(val_, n);
  }
  /// The type of the previous function.
  using conjunction_repeated_t
    = auto (automaton::*)(unsigned n) const -> automaton;

  static automaton conjunction(const boost::python::list& auts,
                               bool lazy = false)
  {
    return vcsn::dyn::conjunction(automata_(auts), lazy);
  }
  /// The type of the previous function.
  using conjunction_variadic_t
    = auto (*)(const boost::python::list& auts, bool lazy) -> automaton;

  ::context context() const
  {
    return vcsn::dyn::context_of(val_);
  }

  automaton costandard() const
  {
    return vcsn::dyn::costandard(val_);
  }

  automaton delay_automaton() const
  {
    return vcsn::dyn::delay_automaton(val_);
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

  weight eval(const label& l) const;

  automaton factor() const
  {
    return vcsn::dyn::factor(val_);
  }

  automaton filter(const boost::python::list& states) const
  {
    return vcsn::dyn::filter(val_, make_vector<unsigned>(states));
  }

  automaton focus(unsigned tape)
  {
    return vcsn::dyn::focus(val_, tape);
  }

  std::string format(const std::string& format = "dot") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  bool has_bounded_lag() const
  {
    return vcsn::dyn::has_bounded_lag(val_);
  }

  bool has_lightening_cycle() const
  {
    return vcsn::dyn::has_lightening_cycle(val_);
  }

  bool has_twins_property() const
  {
    return vcsn::dyn::has_twins_property(val_);
  }

  static automaton infiltration_(const boost::python::list& auts)
  {
    return vcsn::dyn::infiltration(automata_(auts));
  }

  automaton insplit(bool lazy = false) const
  {
    return vcsn::dyn::insplit(val_, lazy);
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

  bool is_letterized() const
  {
    return vcsn::dyn::is_letterized(val_);
  }

  bool is_partial_identity() const
  {
    return vcsn::dyn::is_partial_identity(val_);
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

  bool is_realtime() const
  {
    return vcsn::dyn::is_realtime(val_);
  }

  bool is_standard() const
  {
    return vcsn::dyn::is_standard(val_);
  }

  bool is_synchronized() const
  {
    return vcsn::dyn::is_synchronized(val_);
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

  automaton ldiv(const automaton& rhs) const
  {
    return vcsn::dyn::ldiv(val_, rhs.val_);
  }

  automaton left_mult(const weight& w,
                      const std::string& algo = "auto") const;

  automaton letterize() const
  {
    return vcsn::dyn::letterize(val_);
  }

  automaton lift(const boost::python::list& tapes,
                 const std::string& ids = "default") const
  {
    return vcsn::dyn::lift(val_, make_vector<unsigned>(tapes), identities(ids));
  }

  /// The type of the previous function.
  using lift_tapes_t
    = auto (automaton::*)(const boost::python::list& tapes,
                          const std::string& ids) const -> automaton;

  polynomial lightest(unsigned num, const std::string& algo = "auto") const;

  automaton lightest_automaton(unsigned num = 1U,
                               const std::string& algo = "auto") const
  {
    return vcsn::dyn::lightest_automaton(val_, num, algo);
  }

  automaton minimize(const std::string& algo = "auto") const
  {
    return vcsn::dyn::minimize(val_, algo);
  }

  automaton multiply(const automaton& rhs,
                     const std::string& algo = "auto") const
  {
    return vcsn::dyn::multiply(val_, rhs.val_, algo);
  }
  /// The type of the previous function.
  using multiply_t =
    auto
    (automaton::*)(const automaton&, const std::string& algo) const
      -> automaton;

  automaton multiply(int min, int max, const std::string& algo = "auto") const
  {
    return vcsn::dyn::multiply(val_, min, max, algo);
  }
  /// The type of the previous function.
  using multiply_repeated_t =
    auto
    (automaton::*)(int min, int max, const std::string& algo) const
      -> automaton;

  automaton multiply(int min, const std::string& algo = "auto") const
  {
    return multiply(min, min, algo);
  }

  /// The type of the previous function.
  using multiply_min_t =
    auto
    (automaton::*)(int min, const std::string& algo) const
      -> automaton;

  automaton normalize() const
  {
    return vcsn::dyn::normalize(val_);
  }

  std::size_t num_components() const
  {
    return vcsn::dyn::num_components(val_);
  }

  automaton pair(bool keep_initials = false) const
  {
    return vcsn::dyn::pair(val_, keep_initials);
  }

  automaton partial_identity() const
  {
    return vcsn::dyn::partial_identity(val_);
  }

  automaton prefix() const
  {
    return vcsn::dyn::prefix(val_);
  }

  automaton project(unsigned tape)
  {
    return vcsn::dyn::project(val_, tape);
  }

  automaton proper(bool prune = true, bool backward = true,
                   const std::string& algo = "auto") const
  {
    return vcsn::dyn::proper(val_,
                             backward
                             ? vcsn::direction::backward
                             : vcsn::direction::forward,
                             prune, algo);
  }

  automaton push_weights() const
  {
    return vcsn::dyn::push_weights(val_);
  }

  automaton rdiv(const automaton& rhs) const
  {
    return vcsn::dyn::rdiv(val_, rhs.val_);
  }

  automaton realtime() const
  {
    return vcsn::dyn::realtime(val_);
  }

  automaton reduce() const
  {
    return vcsn::dyn::reduce(val_);
  }

  automaton right_mult(const weight& w,
                       const std::string& algo = "auto") const;

  automaton scc(const std::string& algo = "auto") const
  {
    return vcsn::dyn::scc(val_, algo);
  }

  polynomial shortest(boost::optional<unsigned> num,
                      boost::optional<unsigned> len) const;

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

  automaton star(const std::string& algo = "auto") const
  {
    return vcsn::dyn::star(val_, algo);
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

  automaton sum(const automaton& rhs, const std::string& algo = "auto") const
  {
    return vcsn::dyn::sum(val_, rhs.val_, algo);
  }

  automaton synchronize() const
  {
    return vcsn::dyn::synchronize(val_);
  }

  label synchronizing_word(const std::string& algo = "greedy") const;

  expression to_expression(const std::string& ids = "default",
                           const std::string& algo = "auto") const;

  automaton transpose()
  {
    return vcsn::dyn::transpose(val_);
  }

  automaton trim() const
  {
    return vcsn::dyn::trim(val_);
  }

  std::string type() const
  {
    return vcsn::dyn::type(val_);
  }

  automaton universal() const
  {
    return vcsn::dyn::universal(val_);
  }

  weight weight_series() const;

  /// Convert to a vector of dyn:: automata.
  using automata_t = std::vector<vcsn::dyn::automaton>;
  static automata_t automata_(const boost::python::list& auts)
  {
    auto res = automata_t{};
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

  expansion complement() const
  {
    return vcsn::dyn::complement(val_);
  }

  ::context context() const
  {
    return vcsn::dyn::context_of(val_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  expansion sum(const expansion& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  expansion left_mult(const weight& w) const;

  expansion project(unsigned tape)
  {
    return vcsn::dyn::project(val_, tape);
  }

  expansion right_mult(const weight& w) const;

  static expansion tuple_(const boost::python::list& es)
  {
    return vcsn::dyn::tuple(expansions_(es));
  }

  /// Convert to a vector of dyn:: expansions.
  using expansions_t = std::vector<vcsn::dyn::expansion>;
  static expansions_t expansions_(const boost::python::list& es)
  {
    auto res = expansions_t{};
    for (int i = 0; i < boost::python::len(es); ++i)
      res.emplace_back(boost::python::extract<expansion>(es[i])().val_);
    return res;
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
    val_ = vcsn::dyn::read_label(ctx.val_, is);
    vcsn::require(is.peek() == EOF, "unexpected trailing characters: ", is);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  label multiply(const label& rhs) const
  {
    return vcsn::dyn::multiply(val_, rhs.val_);
  }

  label project(unsigned tape) const
  {
    return vcsn::dyn::project(val_, tape);
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
    val_ = vcsn::dyn::read_polynomial(ctx.val_, is);
    vcsn::require(is.peek() == EOF, "unexpected trailing characters: ", is);
  }

  polynomial conjunction(const polynomial& rhs) const
  {
    return vcsn::dyn::conjunction(val_, rhs.val_);
  }

  automaton cotrie() const
  {
    return vcsn::dyn::cotrie(val_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  polynomial ldiv(const polynomial& rhs) const
  {
    return vcsn::dyn::ldiv(val_, rhs.val_);
  }

  polynomial lgcd(const polynomial& rhs) const
  {
    return vcsn::dyn::lgcd(val_, rhs.val_);
  }

  polynomial multiply(const polynomial& rhs) const
  {
    return vcsn::dyn::multiply(val_, rhs.val_);
  }

  polynomial project(unsigned tape)
  {
    return vcsn::dyn::project(val_, tape);
  }

  polynomial split() const
  {
    return vcsn::dyn::split(val_);
  }

  polynomial sum(const polynomial& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  automaton trie() const
  {
    return vcsn::dyn::trie(val_);
  }

  vcsn::dyn::polynomial val_;
};

/*--------------.
| expression.   |
`--------------*/

struct expression
{
  expression(const vcsn::dyn::expression& r)
    : val_(r)
  {}

  expression(const context& ctx, const std::string& r,
             vcsn::rat::identities ids)
  {
    std::istringstream is(r);
    val_ = vcsn::dyn::read_expression(ctx.val_, ids, is);
    vcsn::require(is.peek() == EOF, "unexpected trailing characters: ", is);
  }

  expression(const context& ctx, const std::string& r,
             const std::string& ids)
    : expression{ctx, r, identities(ids)}
  {}

  /// Parse as a series.
  static expression series(const context& ctx, const std::string& r)
  {
    return expression(ctx, r, vcsn::rat::identities::distributive);
  }

  /// Convert \a this to \a ctx, using \a ids.
  expression as_(const ::context& ctx, vcsn::rat::identities ids = {})
  {
    // The destination expressionset.
    return vcsn::dyn::copy(val_, (ctx ? ctx : context()).val_, ids);
  }

  /// Same expression/series, but in context \a ctx, with expression
  /// identities.
  expression as_expression(const ::context& ctx = {})
  {
    return as_(ctx);
  }

  /// Same expression/series, but in context \a ctx, with series identities.
  expression as_series(const ::context& ctx = {})
  {
    return as_(ctx, vcsn::rat::identities::distributive);
  }

  expression complement() const
  {
    return vcsn::dyn::complement(val_);
  }

  expression conjunction(const expression& rhs) const
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

  automaton derived_term(const std::string& algo = "auto") const
  {
    return vcsn::dyn::derived_term(val_, algo);
  }

  expression difference(const expression& rhs) const
  {
    return vcsn::dyn::difference(val_, rhs.val_);
  }

  expression expand() const
  {
    return vcsn::dyn::expand(val_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  expression infiltration(const expression& rhs)
  {
    return vcsn::dyn::infiltration(val_, rhs.val_);
  }

  bool is_equivalent(const expression& rhs) const
  {
    return vcsn::dyn::are_equivalent(val_, rhs.val_);
  }

  bool is_series() const
  {
    return vcsn::dyn::identities(val_).is_distributive();
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(val_);
  }

  expression ldiv(const expression& rhs) const
  {
    return vcsn::dyn::ldiv(val_, rhs.val_);
  }

  expression left_mult(const weight& w) const;

  bool less_than(const expression& rhs) const
  {
    return vcsn::dyn::less_than(val_, rhs.val_);
  }

  expression lift() const
  {
    return vcsn::dyn::lift(val_);
  }

  expression multiply(const expression& rhs) const
  {
    return vcsn::dyn::multiply(val_, rhs.val_);
  }
  /// The type of the previous function.
  using multiply_t
    = auto (expression::*)(const expression&) const -> expression;

  expression multiply(int min, int max) const
  {
    return vcsn::dyn::multiply(val_, min, max);
  }
  /// The type of the previous function.
  using multiply_repeated_t
    = auto (expression::*)(int min, int max) const -> expression;

  expression multiply(int min) const
  {
    return multiply(min, min);
  }

  expression project(unsigned tape)
  {
    return vcsn::dyn::project(val_, tape);
  }

  expression rdiv(const expression& rhs) const
  {
    return vcsn::dyn::rdiv(val_, rhs.val_);
  }

  expression right_mult(const weight& w) const;

  expression shuffle(const expression& rhs)
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

  expression star_normal_form() const
  {
    return vcsn::dyn::star_normal_form(val_);
  }

  expression sum(const expression& rhs) const
  {
    return vcsn::dyn::sum(val_, rhs.val_);
  }

  automaton thompson() const
  {
    return vcsn::dyn::thompson(val_);
  }

  automaton to_automaton(const std::string& algo = "auto") const
  {
    return vcsn::dyn::to_automaton(val_, algo);
  }

  expansion to_expansion() const
  {
    return vcsn::dyn::to_expansion(val_);
  }

  expression transpose() const
  {
    return vcsn::dyn::transpose(val_);
  }

  expression transposition() const
  {
    return vcsn::dyn::transposition(val_);
  }

  static expression tuple_(const boost::python::list& es)
  {
    return vcsn::dyn::tuple(expressions_(es));
  }

  automaton zpc(const std::string& algo = "auto") const
  {
    return vcsn::dyn::zpc(val_, algo);
  }

  /// Convert to a vector of dyn:: expressions.
  using expressions_t = std::vector<vcsn::dyn::expression>;
  static expressions_t expressions_(const boost::python::list& es)
  {
    auto res = expressions_t{};
    for (int i = 0; i < boost::python::len(es); ++i)
      res.emplace_back(boost::python::extract<expression>(es[i])().val_);
    return res;
  }

  vcsn::dyn::expression val_;
};

expression context::series(const std::string& s) const
{
  return expression::series(*this, s);
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
    val_ = vcsn::dyn::read_weight(ctx.val_, is);
    vcsn::require(is.peek() == EOF, "unexpected trailing characters: ", is);
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
  /// The type of the previous function.
  using multiply_t
    = auto (weight::*)(const weight&) const -> weight;

  weight multiply(int min, int max) const
  {
    return vcsn::dyn::multiply(val_, min, max);
  }
  /// The type of the previous function.
  using multiply_repeated_t =
    auto (weight::*)(int min, int max) const -> weight;

  weight multiply(int min) const
  {
    return multiply(min, min);
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

automaton::automaton(const expression& r)
  : val_(nullptr)
{
  *this = r.derived_term();
}

label automaton::ambiguous_word() const
{
  return vcsn::dyn::ambiguous_word(val_);
}

weight automaton::eval(const label& l) const
{
  return vcsn::dyn::eval(val_, l.val_);
}

bool automaton::is_synchronized_by(const label& word) const
{
  return vcsn::dyn::is_synchronized_by(val_, word.val_);
}

automaton automaton::left_mult(const weight& w,
                               const std::string& algo) const
{
  return vcsn::dyn::left_mult(w.val_, val_, algo);
}

polynomial automaton::lightest(unsigned num, const std::string& algo) const
{
  return vcsn::dyn::lightest(val_, num, algo);
}

automaton automaton::right_mult(const weight& w,
                                const std::string& algo) const
{
  return vcsn::dyn::right_mult(val_, w.val_, algo);
}

polynomial automaton::shortest(boost::optional<unsigned> num,
                               boost::optional<unsigned> len) const
{
  return vcsn::dyn::shortest(val_, num, len);
}

label automaton::synchronizing_word(const std::string& algo) const
{
  return vcsn::dyn::synchronizing_word(val_, algo);
}

expression automaton::to_expression(const std::string& ids,
                                    const std::string& algo) const
{
  return vcsn::dyn::to_expression(val_, identities(ids), algo);
}

weight automaton::weight_series() const
{
  return vcsn::dyn::weight_series(val_);
}

/*-------------------------.
| context implementation.  |
`-------------------------*/

automaton context::cerny(unsigned n) const
{
  return vcsn::dyn::cerny(val_, n);
}

automaton context::cotrie(const std::string& data,
                          const std::string& format,
                          const std::string& filename) const
{
  auto is = make_istream(data, filename);
  auto res = vcsn::dyn::cotrie(val_, *is, format);
  vcsn::require(is->peek() == EOF, "unexpected trailing characters: ", *is);
  return res;
}

automaton context::de_bruijn(unsigned n) const
{
  return vcsn::dyn::de_bruijn(val_, n);
}

automaton context::divkbaseb(unsigned divisor, unsigned base) const
{
  return vcsn::dyn::divkbaseb(val_, divisor, base);
}

automaton context::double_ring(unsigned n,
                               const boost::python::list& finals) const
{
  return vcsn::dyn::double_ring(val_, n, make_vector<unsigned>(finals));
}

automaton context::ladybird(unsigned n) const
{
  return vcsn::dyn::ladybird(val_, n);
}

automaton context::levenshtein() const
{
  return vcsn::dyn::levenshtein(val_);
}

context context::project(unsigned tape) const
{
  return vcsn::dyn::project(val_, tape);
}

automaton context::random(unsigned num_states, float density,
                          unsigned num_initial, unsigned num_final,
                          boost::optional<unsigned> max_labels,
                          float loop_chance) const
{
  return vcsn::dyn::random_automaton(val_,
                                     num_states, density,
                                     num_initial, num_final,
                                     max_labels, loop_chance);
}

automaton context::random_deterministic(unsigned num_states) const
{
  return vcsn::dyn::random_automaton_deterministic(val_, num_states);
}

automaton context::trie(const std::string& data,
                        const std::string& format,
                        const std::string& filename) const
{
  auto is = make_istream(data, filename);
  auto res = vcsn::dyn::trie(val_, *is, format);
  vcsn::require(is->peek() == EOF, "unexpected trailing characters: ", *is);
  return res;
}

automaton context::u(unsigned num_states) const
{
  return vcsn::dyn::u(val_, num_states);
}

label context::word(const std::string& s) const
{
  return label(context(vcsn::dyn::make_word_context(val_)), s);
}

/*----------------------------.
| expansion implementation.   |
`----------------------------*/

expansion expansion::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, val_);
}

expansion expansion::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(val_, w.val_);
}

/*-----------------------------.
| expression implementation.   |
`-----------------------------*/

weight expression::constant_term() const
{
  return vcsn::dyn::constant_term(val_);
}

expression expression::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, val_);
}

expression expression::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(val_, w.val_);
}


/*-----------.
| vcsn_cxx.  |
`-----------*/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(lift,
                                       lift, 0, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(multiply_repeated,
                                       multiply, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(multiply_repeated_automaton,
                                       multiply, 1, 3);

BOOST_PYTHON_MODULE(vcsn_cxx)
{
  namespace bp = boost::python;
  using bp::arg;

  // Activate support for boost::optional.
  python_optional<unsigned>();

  // We use bp::no_init to disable the use of the default ctor from
  // our classes, and prefer to use "def(init<...>)" to define our
  // ctors.  It's easier this way to provide default arguments.

  bp::class_<automaton>("automaton", bp::no_init)
    .def(bp::init<const std::string&, const std::string&,
         const std::string&, bool>
         ((arg("data") = "", arg("format") = "default",
           arg("filename") = "", arg("strip") = true)))
    .def("accessible", &automaton::accessible)
    .def("ambiguous_word", &automaton::ambiguous_word)
    .def("automaton", &automaton::as)
    .def("focus", &automaton::focus)
    .def("coaccessible", &automaton::coaccessible)
    .def("codeterminize", &automaton::codeterminize, (arg("algo") = "auto"))
    .def("cominimize", &automaton::cominimize, (arg("algo") = "auto"))
    .def("complement", &automaton::complement)
    .def("complete", &automaton::complete)
    .def("component", &automaton::component)
    .def("compose", &automaton::compose, (arg("lazy") = false))
    .def("condense", &automaton::condense)
    .def("conjunction",
         static_cast<automaton::conjunction_repeated_t>(&automaton::conjunction))
    .def("conjunction",
         static_cast<automaton::conjunction_variadic_t>(&automaton::conjunction),
         (arg("lazy") = false))
        .staticmethod("conjunction")
    .def("conjugate", &automaton::conjugate)
    .def("context", &automaton::context)
    .def("costandard", &automaton::costandard)
    .def("delay_automaton", &automaton::delay_automaton)
    .def("determinize", &automaton::determinize, (arg("algo") = "auto"))
    .def("difference", &automaton::difference)
    .def("eliminate_state", &automaton::eliminate_state, (arg("state") = -1))
    .def("_eval", &automaton::eval)
    .def("factor", &automaton::factor)
    .def("filter", &automaton::filter)
    .def("_format", &automaton::format)
    .def("has_bounded_lag", &automaton::has_bounded_lag)
    .def("has_lightening_cycle", &automaton::has_lightening_cycle)
    .def("has_twins_property", &automaton::has_twins_property)
    .def("_infiltration", &automaton::infiltration_).staticmethod("_infiltration")
    .def("insplit", &automaton::insplit, (arg("lazy") = false))
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
    .def("is_letterized", &automaton::is_letterized)
    .def("is_partial_identity", &automaton::is_partial_identity)
    .def("is_isomorphic", &automaton::is_isomorphic)
    .def("is_normalized", &automaton::is_normalized)
    .def("is_proper", &automaton::is_proper)
    .def("is_out_sorted", &automaton::is_out_sorted)
    .def("is_realtime", &automaton::is_realtime)
    .def("is_standard", &automaton::is_standard)
    .def("is_synchronized", &automaton::is_synchronized)
    .def("_is_synchronized_by", &automaton::is_synchronized_by)
    .def("is_synchronizing", &automaton::is_synchronizing)
    .def("is_trim", &automaton::is_trim)
    .def("is_useless", &automaton::is_useless)
    .def("is_valid", &automaton::is_valid)
    .def("ldiv", &automaton::ldiv)
    .def("left_mult", &automaton::left_mult,
         (arg("weight"), arg("algo") = "auto"))
    .def("letterize", &automaton::letterize)
    .def("_lift", &automaton::lift)
    .def("lightest", &automaton::lightest,
         (arg("num") = 1U, arg("algo") = "auto"))
    .def("lightest_automaton",
         &automaton::lightest_automaton, (arg("num") = 1U, arg("algo") = "auto"))
    .def("minimize", &automaton::minimize, (arg("algo") = "auto"))
    .def("multiply", static_cast<automaton::multiply_t>(&automaton::multiply),
         (arg("algo") = "auto"))
    .def("multiply",
         static_cast<automaton::multiply_repeated_t>(&automaton::multiply),
         multiply_repeated_automaton(bp::args("min", "max", "algo")))
    .def("multiply",
         static_cast<automaton::multiply_min_t>(&automaton::multiply),
         multiply_repeated(bp::args("min", "algo")))
    .def("normalize", &automaton::normalize)
    .def("num_components", &automaton::num_components)
    .def("pair", &automaton::pair, (arg("keep_initials") = false))
    .def("prefix", &automaton::prefix)
    .def("partial_identity", &automaton::partial_identity)
    .def("project", &automaton::project)
    .def("proper", &automaton::proper,
         (arg("prune") = true, arg("backward") = true, arg("algo") = "auto"))
    .def("push_weights", &automaton::push_weights)
    .def("realtime", &automaton::realtime)
    .def("expression", &automaton::to_expression,
         (arg("identities") = "default", arg("algo") = "auto"))
    .def("rdiv", &automaton::rdiv)
    .def("reduce", &automaton::reduce)
    .def("right_mult", &automaton::right_mult,
         (arg("weight"), arg("algo") = "auto"))
    .def("scc", &automaton::scc, (arg("algo") = "auto"))
    .def("shortest", &automaton::shortest,
         (arg("num") = boost::optional<unsigned>(),
          arg("len") = boost::optional<unsigned>()))
    .def("_shuffle", &automaton::shuffle_).staticmethod("_shuffle")
    .def("sort", &automaton::sort)
    .def("standard", &automaton::standard)
    .def("star", &automaton::star, (arg("algo") = "auto"))
    .def("strip", &automaton::strip)
    .def("suffix", &automaton::suffix)
    .def("subword", &automaton::subword)
    .def("sum", &automaton::sum, (arg("algo") = "auto"))
    .def("synchronize", &automaton::synchronize)
    .def("synchronizing_word",
         &automaton::synchronizing_word, (arg("algo") = "greedy"))
    .def("transpose", &automaton::transpose)
    .def("trim", &automaton::trim)
    .def("type", &automaton::type)
    .def("universal", &automaton::universal)
    .def("weight_series", &automaton::weight_series)
    ;

  bp::class_<context>("context", bp::no_init)
    .def(bp::init<const std::string&>())
    .def("cerny", &context::cerny)
    .def("cotrie", &context::cotrie,
         (arg("data") = "", arg("format") = "default",
          arg("filename") = ""))
    .def("de_bruijn", &context::de_bruijn)
    .def("divkbaseb", &context::divkbaseb)
    .def("double_ring", &context::double_ring)
    .def("format", &context::format)
    .def("join", &context::join)
    .def("ladybird", &context::ladybird)
    .def("levenshtein", &context::levenshtein)
    .def("project", &context::project)
    .def("random", &context::random,
         (arg("num_states"), arg("density") = 0.1,
          arg("num_initial") = 1, arg("num_final") = 1,
          arg("max_labels") = boost::optional<unsigned>(),
          arg("loop_chance") = 0))
    .def("random_deterministic", &context::random_deterministic)
    .def("series", &context::series)
    .def("trie", &context::trie,
         (arg("data") = "", arg("format") = "default",
          arg("filename") = ""))
    .def("u", &context::u)
    .def("word", &context::word)
   ;

  bp::class_<expansion>("expansion", bp::no_init)
    .def(bp::init<const std::string&>())
    .def("complement", &expansion::complement)
    .def("context", &expansion::context)
    .def("format", &expansion::format)
    .def("left_mult", &expansion::left_mult)
    .def("project", &expansion::project)
    .def("right_mult", &expansion::right_mult)
    .def("sum", &expansion::sum)
    .def("_tuple", &expansion::tuple_).staticmethod("_tuple")
   ;

  bp::class_<expression>("expression", bp::no_init)
    .def(bp::init<const context&, const std::string&, const std::string&>
         ((arg("context"), arg("data"), arg("identities") = "default")))
    .def("automaton", &expression::to_automaton, (arg("algo") = "auto"))
    .def("complement", &expression::complement)
    .def("conjunction", &expression::conjunction)
    .def("constant_term", &expression::constant_term)
    .def("context", &expression::context)
    .def("_derivation", &expression::derivation,
         (arg("label"), arg("breaking") = false))
    .def("derived_term", &expression::derived_term, (arg("algo") = "auto"))
    .def("difference", &expression::difference)
    .def("expand", &expression::expand)
    .def("expansion", &expression::to_expansion)
    .def("expression", &expression::as_expression,
         (arg("context") = context()))
    .def("format", &expression::format)
    .def("infiltration", &expression::infiltration)
    .def("is_equivalent", &expression::is_equivalent)
    .def("is_series", &expression::is_series)
    .def("is_valid", &expression::is_valid)
    .def("ldiv", &expression::ldiv)
    .def("left_mult", &expression::left_mult)
    .def("less_than", &expression::less_than)
    .def("lift", &expression::lift)
    .def("multiply", static_cast<expression::multiply_t>(&expression::multiply))
    .def("multiply",
         static_cast<expression::multiply_repeated_t>(&expression::multiply),
         multiply_repeated())
    .def("project", &expression::project)
    .def("rdiv", &expression::rdiv)
    .def("right_mult", &expression::right_mult)
    .def("series", &expression::as_series, (arg("context") = context()))
    .def("shuffle", &expression::shuffle)
    .def("split", &expression::split)
    .def("standard", &expression::standard)
    .def("star_height", &expression::star_height)
    .def("star_normal_form", &expression::star_normal_form)
    .def("sum", &expression::sum)
    .def("thompson", &expression::thompson)
    .def("transpose", &expression::transpose)
    .def("transposition", &expression::transposition)
    .def("_tuple", &expression::tuple_).staticmethod("_tuple")
    .def("zpc", &expression::zpc, (arg("algo") = "auto"))
    ;

  bp::class_<label>("label", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("format", &label::format)
    .def("multiply", &label::multiply)
    .def("project", &label::project)
   ;

  bp::class_<polynomial>("polynomial", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("conjunction", &polynomial::conjunction)
    .def("cotrie", &polynomial::cotrie)
    .def("format", &polynomial::format)
    .def("ldiv", &polynomial::ldiv)
    .def("lgcd", &polynomial::lgcd)
    .def("multiply", &polynomial::multiply)
    .def("project", &polynomial::project)
    .def("split", &polynomial::split)
    .def("sum", &polynomial::sum)
    .def("trie", &polynomial::trie)
   ;

  bp::class_<weight>("weight", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("format", &weight::format)
    .def("multiply", static_cast<weight::multiply_t>(&weight::multiply))
    .def("multiply",
         static_cast<weight::multiply_repeated_t>(&weight::multiply),
         multiply_repeated())
    .def("sum", &weight::sum)
   ;
}
