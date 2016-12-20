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
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/raise.hh>

#include "python/oodyn.hh"

/// The type of the repeated conjunction function.
using automaton_conjunction_repeated_t
  = auto (automaton::*)(unsigned n) const -> automaton;

using automaton_copy_t
  = auto (automaton::*)(const context&) const -> automaton;

using evaluate_t
  = auto (automaton::*)(const label&) const -> weight;

using evaluate_polynomial_t
  = auto (automaton::*)(const polynomial&) const -> weight;

/// The type of the binary multiply function for automata.
using automaton_multiply_t
  = auto
    (automaton::*)(const automaton&, const std::string& algo) const
    -> automaton;
/// The type of the repeated multiply function for automata.
using automaton_multiply_repeated_t =
  auto
  (automaton::*)(int min, int max, const std::string& algo) const
    -> automaton;

/// The type of the binary multiply function for expressions and
/// weights.
template <typename Value>
using multiply_t
  = auto (Value::*)(const Value&) const -> Value;
/// The type of the repeated multiply function for expressions and
/// weights.
template <typename Value>
using multiply_repeated_t =
  auto (Value::*)(int min, int max) const -> Value;
/// The type of the repeated multiply function for labels.
using label_multiply_repeated_t =
  auto (label::*)(int exp) const -> label;

/// The type of string-based trie/cotrie.
using string_trie_t =
  auto (context::*)(const std::string&, const std::string&,
                    const std::string&) const
    -> automaton;

/// Convert a Python list to a C++ vector.
template <typename T>
auto make_vector(const boost::python::list& list)
  -> std::vector<T>
{
  auto res = std::vector<T>{};
  for (int i = 0; i < boost::python::len(list); ++i)
    res.emplace_back(boost::python::extract<T>(list[i]));
  return res;
}

/// Convert a Python list to a C++ vector.
template <typename T>
auto make_value_vector(const boost::python::list& list)
{
  auto res = std::vector<decltype(std::declval<T>().val_)>{};
  for (auto v: make_vector<T>(list))
    res.emplace_back(v.val_);
  return res;
}

/// Convert this value to string.
template <typename Value>
std::string format(const Value& v,
                   const std::string& format = "text")
{
  std::ostringstream os;
  vcsn::dyn::print(v.val_, os, format);
  return os.str();
}

label context_word(const context& ctx, const std::string& s)
{
  return label(context(vcsn::dyn::make_word_context(ctx.val_)), s);
}

expression context_random_expression(const context& ctx,
                                     const std::string& param,
                                     const std::string& ids)
{
  return ctx.random_expression(param, ids);
}

automaton automaton_conjunction(const boost::python::list& l,
                                bool lazy = false)
{
  return automaton::conjunction(make_vector<automaton>(l), lazy);
}

automaton automaton_infiltrate(const boost::python::list& l)
{
  return automaton::infiltrate(make_vector<automaton>(l));
}

automaton automaton_filter(const automaton& aut,
                           const boost::python::list& states)
{
  return aut.filter(make_vector<unsigned>(states));
}

automaton automaton_lift(const automaton& aut,
                         const boost::python::list& tapes,
                         const std::string& ids = "default")
{
  return aut.lift(make_vector<unsigned>(tapes), ids);
}

automaton automaton_shuffle(const boost::python::list& l)
{
  return automaton::shuffle(make_vector<automaton>(l));
}

expression automaton_expression(const automaton& aut,
                                const std::string& ids = "default",
                                const std::string& algo = "auto")
{
  return aut.to_expression(ids, algo);
}

automaton automaton_tuple(const boost::python::list& l)
{
  return automaton::tuple(make_vector<automaton>(l));
}

automaton context_double_ring(const context& ctx, unsigned n,
                              const boost::python::list& finals)
{
  return ctx.double_ring(n, make_vector<unsigned>(finals));
}

context context_tuple(const boost::python::list& l)
{
  return context::tuple(make_vector<context>(l));
}

expansion expansion_tuple(const boost::python::list& l)
{
  return expansion::tuple(make_vector<expansion>(l));
}

expression expression_tuple(const boost::python::list& l)
{
  return expression::tuple(make_vector<expression>(l));
}

polynomial polynomial_tuple(const boost::python::list& l)
{
  return polynomial::tuple(make_vector<polynomial>(l));
}


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
    void *const storage = reinterpret_cast<data_t*>(data)->storage.bytes;
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

/// Bidirectional conversion for `EnumType` as a Python string.
template <typename EnumType>
struct python_string__enum
  : private boost::noncopyable
{
  struct conversion
  {
    static PyObject* convert(EnumType const& s)
    {
      return PyUnicode_FromString(to_string(s).c_str());
    }
  };

  // Determine if obj_ptr can be converted in `EnumType`.
  static void* convertible(PyObject* obj_ptr)
  {
    return PyUnicode_Check(obj_ptr) ? obj_ptr : nullptr;
  }

  // Convert obj_ptr into an `EnumType`.
  static void
  constructor(PyObject* obj,
              boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    namespace bp = boost::python;

    // Extract the character data from the python string
    const char* value = PyBytes_AsString(PyUnicode_AsASCIIString(obj));

    // Verify that obj is a string (should be ensured by convertible()).
    assert(value);

    // Grab pointer to memory into which to construct the new `EnumType` value.
    using data_t = bp::converter::rvalue_from_python_storage<EnumType>;
    void* storage = reinterpret_cast<data_t*>(data)->storage.bytes;

    // In-place construct the new `EnumType` using the character
    // data extracted from the Python object.
    {
      std::istringstream is{value};
      EnumType e;
      is >> e;
      VCSN_REQUIRE(is.peek() == EOF,
                   "invalid value: ", value,
                   ", unexpected ", vcsn::str_escape(is.peek()));
      new (storage) EnumType{e};
    }

    // Stash the memory chunk pointer for later use by Boost.Python.
    data->convertible = storage;
  }

  explicit python_string__enum()
  {
    namespace bp = boost::python;
    if (!bp::extract<python_string__enum>(bp::object()).check())
      {
        bp::to_python_converter<EnumType, conversion>();
        bp::converter::registry::push_back
          (&convertible,
           &constructor,
           bp::type_id<EnumType>());
      }
    }
};


/*-----------.
| vcsn_cxx.  |
`-----------*/

BOOST_PYTHON_MODULE(vcsn_cxx)
{
  namespace bp = boost::python;
  using bp::arg;

  // Activate support for boost::optional and identities.
  python_optional<unsigned>();
  python_string__enum<vcsn::dyn::direction>();
  python_string__enum<vcsn::dyn::identities>();

  // Free functions.
  bp::def("configuration", &vcsn::dyn::configuration);

  // We use bp::no_init to disable the use of the default ctor from
  // our classes, and prefer to use "def(init<...>)" to define our
  // ctors.  It's easier this way to provide default arguments.

  bp::class_<automaton>("automaton", bp::no_init)
    .def(bp::init<const std::string&, const std::string&,
         const std::string&, bool>
         ((arg("data") = "", arg("format") = "default",
           arg("filename") = "", arg("strip") = true)))
    .def("accessible", &automaton::accessible)
    .def("add", &automaton::add, (arg("algo") = "auto"))
    .def("ambiguous_word", &automaton::ambiguous_word)
    .def("automaton", static_cast<automaton_copy_t>(&automaton::copy))
    .def("focus", &automaton::focus)
    .def("coaccessible", &automaton::coaccessible)
    .def("codeterminize", &automaton::codeterminize, (arg("algo") = "auto"))
    .def("cominimize", &automaton::cominimize, (arg("algo") = "auto"))
    .def("compare", &automaton::compare)
    .def("complement", &automaton::complement)
    .def("complete", &automaton::complete)
    .def("component", &automaton::component)
    .def("compose", &automaton::compose, (arg("lazy") = false))
    .def("condense", &automaton::condense)
    .def("conjunction",
         static_cast<automaton_conjunction_repeated_t>(&automaton::conjunction))
    .def("conjunction", &automaton_conjunction,
         (arg("automata"), arg("lazy") = false))
        .staticmethod("conjunction")
    .def("conjugate", &automaton::conjugate)
    .def("context", &automaton::context)
    .def("costandard", &automaton::costandard)
    .def("delay_automaton", &automaton::delay_automaton)
    .def("determinize", &automaton::determinize, (arg("algo") = "auto"))
    .def("difference", &automaton::difference)
    .def("eliminate_state", &automaton::eliminate_state, (arg("state") = -1))
    .def("_evaluate", static_cast<evaluate_t>(&automaton::evaluate))
    .def("_evaluate", static_cast<evaluate_polynomial_t>(&automaton::evaluate))
    .def("factor", &automaton::factor)
    .def("filter", &automaton_filter)
    .def("_format", &format<automaton>)
    .def("has_bounded_lag", &automaton::has_bounded_lag)
    .def("has_lightening_cycle", &automaton::has_lightening_cycle)
    .def("has_twins_property", &automaton::has_twins_property)
    .def("_infiltrate", &automaton_infiltrate).staticmethod("_infiltrate")
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
    .def("ldivide", &automaton::ldivide)
    .def("less_than", &automaton::less_than)
    .def("lweight", &automaton::lweight,
         (arg("weight"), arg("algo") = "auto"))
    .def("letterize", &automaton::letterize)
    .def("_lift", &automaton_lift)
    .def("_lift", &automaton::lift)
    .def("lightest", &automaton::lightest,
         (arg("num") = 1U, arg("algo") = "auto"))
    .def("lightest_automaton",
         &automaton::lightest_automaton,
         (arg("num") = 1U, arg("algo") = "auto"))
    .def("minimize", &automaton::minimize, (arg("algo") = "auto"))
    .def("multiply", static_cast<automaton_multiply_t>(&automaton::multiply),
         (arg("algo") = "auto"))
    .def("multiply",
         static_cast<automaton_multiply_repeated_t>(&automaton::multiply),
         (arg("min"), arg("max") = -2, arg("algo") = "auto"))
    .def("normalize", &automaton::normalize)
    .def("num_components", &automaton::num_components)
    .def("pair", &automaton::pair, (arg("keep_initials") = false))
    .def("prefix", &automaton::prefix)
    .def("partial_identity", &automaton::partial_identity)
    .def("project", &automaton::project)
    .def("proper", &automaton::proper,
         (arg("direction") = "backward", arg("prune") = true,
          arg("algo") = "auto"))
    .def("push_weights", &automaton::push_weights)
    .def("realtime", &automaton::realtime)
    .def("expression", &automaton_expression,
         (arg("identities") = "default", arg("algo") = "auto"))
    .def("rdivide", &automaton::rdivide)
    .def("reduce", &automaton::reduce)
    .def("rweight", &automaton::rweight,
         (arg("weight"), arg("algo") = "auto"))
    .def("scc", &automaton::scc, (arg("algo") = "auto"))
    .def("shortest", &automaton::shortest,
         (arg("num") = boost::optional<unsigned>(),
          arg("len") = boost::optional<unsigned>()))
    .def("_shuffle", &automaton_shuffle).staticmethod("_shuffle")
    .def("sort", &automaton::sort)
    .def("standard", &automaton::standard)
    .def("star", &automaton::star, (arg("algo") = "auto"))
    .def("strip", &automaton::strip)
    .def("suffix", &automaton::suffix)
    .def("subword", &automaton::subword)
    .def("synchronize", &automaton::synchronize)
    .def("synchronizing_word",
         &automaton::synchronizing_word, (arg("algo") = "greedy"))
    .def("transpose", &automaton::transpose)
    .def("trim", &automaton::trim)
    .def("_tuple", &automaton_tuple).staticmethod("_tuple")
    .def("type", &automaton::type)
    .def("universal", &automaton::universal)
    .def("weight_series", &automaton::weight_series)
    ;

  bp::class_<context>("context", bp::no_init)
    .def(bp::init<const std::string&>())
    .def("cerny", &context::cerny)
    .def("cotrie", static_cast<string_trie_t>(&context::cotrie),
         (arg("data") = "", arg("format") = "default",
          arg("filename") = ""))
    .def("de_bruijn", &context::de_bruijn)
    .def("divkbaseb", &context::divkbaseb)
    .def("double_ring", &context_double_ring)
    .def("format", &format<context>)
    .def("join", &context::join)
    .def("ladybird", &context::ladybird)
    .def("levenshtein", &context::levenshtein)
    .def("project", &context::project)
    .def("quotkbaseb", &context::quotkbaseb)
    .def("random_automaton", &context::random_automaton,
         (arg("num_states"), arg("density") = 0.1,
          arg("num_initial") = 1, arg("num_final") = 1,
          arg("max_labels") = boost::optional<unsigned>(),
          arg("loop_chance") = 0,
          arg("weights") = std::string("")))
    .def("random_deterministic", &context::random_automaton_deterministic)
    .def("random_expression", &context_random_expression,
         (arg("parameters") = "", arg("identities") = "default"))
    .def("random_weight", &context::random_weight,
         (arg("parameters") = ""))
    .def("trie", static_cast<string_trie_t>(&context::trie),
         (arg("data") = "", arg("format") = "default",
          arg("filename") = ""))
    .def("_tuple", &context_tuple).staticmethod("_tuple")
    .def("u", &context::u)
    .def("weight_one", &context::weight_one)
    .def("weight_zero", &context::weight_zero)
    .def("word", &context_word)
    .def("word_context", &context::make_word_context)
   ;

  bp::class_<expansion>("expansion", bp::no_init)
    .def("add", &expansion::add)
    .def("complement", &expansion::complement)
    .def("compose", &expansion::compose)
    .def("conjunction", &expansion::conjunction)
    .def("context", &expansion::context)
    .def("format", &format<expansion>)
    .def("ldivide", &expansion::ldivide)
    .def("lweight", &expansion::lweight)
    .def("project", &expansion::project)
    .def("rweight", &expansion::rweight)
    .def("_tuple", &expansion_tuple).staticmethod("_tuple")
   ;

  bp::class_<expression>("expression", bp::no_init)
    .def(bp::init<const context&, const std::string&, const std::string&>
         ((arg("context"), arg("data"), arg("identities") = "default")))
    // `expression.automaton` is redefined to be a native Python function
    .def("_automaton", &expression::to_automaton, (arg("algo") = "auto"))
    .def("add", &expression::add)
    .def("compare", &expression::compare)
    .def("complement", &expression::complement)
    .def("compose", &expression::compose)
    .def("conjunction", &expression::conjunction)
    .def("constant_term", &expression::constant_term)
    .def("context", &expression::context)
    .def("_derivation", &expression::derivation,
         (arg("label"), arg("breaking") = false))
    .def("derived_term", &expression::derived_term, (arg("algo") = "auto"))
    .def("difference", &expression::difference)
    .def("expand", &expression::expand)
    .def("expansion", &expression::to_expansion)
    .def("expression", &expression::as,
         (arg("context") = context(), arg("identities") = "default"))
    .def("format", &format<expression>)
    .def("identities", &expression::identities_of)
    .def("inductive", &expression::inductive, (arg("algo") = "auto"))
    .def("infiltrate", &expression::infiltrate)
    .def("is_equivalent", &expression::is_equivalent)
    .def("is_valid", &expression::is_valid)
    .def("ldivide", &expression::ldivide)
    .def("lweight", &expression::lweight)
    .def("less_than", &expression::less_than)
    .def("lift", &expression::lift)
    .def("multiply", static_cast<multiply_t<expression>>(&expression::multiply))
    .def("multiply",
         static_cast<multiply_repeated_t<expression>>(&expression::multiply),
         (arg("min"), arg("max") = -2))
    .def("partial_identity", &expression::partial_identity)
    .def("project", &expression::project)
    .def("rdivide", &expression::rdivide)
    .def("rweight", &expression::rweight)
    .def("shuffle", &expression::shuffle)
    .def("split", &expression::split)
    .def("standard", &expression::standard)
    .def("star_height", &expression::star_height)
    .def("star_normal_form", &expression::star_normal_form)
    .def("thompson", &expression::thompson)
    .def("transpose", &expression::transpose)
    .def("transposition", &expression::transposition)
    .def("_tuple", &expression_tuple).staticmethod("_tuple")
    .def("zpc", &expression::zpc, (arg("algo") = "auto"))
    ;

  bp::class_<label>("label", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("format", &format<label>)
    .def("ldivide", &label::ldivide)
    .def("multiply", static_cast<multiply_t<label>>(&label::multiply))
    .def("multiply",
         static_cast<label_multiply_repeated_t>(&label::multiply),
         (arg("exp")))
    .def("project", &label::project)
    .def("rdivide", &label::rdivide)
   ;

  bp::class_<polynomial>("polynomial", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("add", &polynomial::add)
    .def("compose", &polynomial::compose)
    .def("conjunction", &polynomial::conjunction)
    .def("context", &polynomial::context)
    .def("cotrie", &polynomial::cotrie)
    .def("format", &format<polynomial>)
    .def("ldivide", &polynomial::ldivide)
    .def("lweight", &polynomial::lweight)
    .def("lgcd", &polynomial::lgcd)
    .def("multiply", &polynomial::multiply)
    .def("project", &polynomial::project)
    .def("rweight", &polynomial::rweight)
    .def("split", &polynomial::split)
    .def("trie", &polynomial::trie)
    .def("_tuple", &polynomial_tuple).staticmethod("_tuple")
   ;

  bp::class_<weight>("weight", bp::no_init)
    .def(bp::init<const context&, const std::string&>())
    .def("add", &weight::add)
    .def("format", &format<weight>)
    .def("ldivide", &weight::ldivide)
    .def("multiply", static_cast<multiply_t<weight>>(&weight::multiply))
    .def("multiply",
         static_cast<multiply_repeated_t<weight>>(&weight::multiply),
         (arg("min"), arg("max") = -2))
    .def("rdivide", &weight::rdivide)
   ;
}
