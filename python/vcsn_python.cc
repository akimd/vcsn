#if 0

sudo port install py27-ipython py27-zmq py27-jinja2 py27-tornado
ipython-2.7 notebook --pylab=inline

#endif

#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <boost/python.hpp>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh> // vname

struct automaton;
struct context;
struct polynomial;
struct ratexp;
struct weight;


/*----------.
| context.  |
`----------*/

struct context
{
  context(const std::string& ctx)
    : ctx_(vcsn::dyn::make_context(ctx))
  {}

  std::string __repr__() const
  {
    // FIXME: we should have vcsn::dyn::format(context).
    return ctx_->vname();
  }

  std::string format(const std::string& format = "latex") const
  {
    std::ostringstream os;
    vcsn::dyn::print(ctx_, os, format);
    return os.str();
  }

  vcsn::dyn::context ctx_;
};


/*------------.
| automaton.  |
`------------*/

struct automaton
{
  automaton(const vcsn::dyn::automaton& a)
    : aut_(a)
  {}

  automaton(const ratexp& r);

  static automaton ladybird(const context& ctx, unsigned n);

  std::string __repr__() const
  {
    return format();
  }

  automaton accessible() const
  {
    return vcsn::dyn::accessible(aut_);
  }

  automaton coaccessible() const
  {
    return vcsn::dyn::coaccessible(aut_);
  }

  automaton complement() const
  {
    return vcsn::dyn::complement(aut_);
  }

  automaton complete() const
  {
    return vcsn::dyn::complete(aut_);
  }

  automaton concatenate(const automaton& rhs) const
  {
    return vcsn::dyn::concatenate(aut_, rhs.aut_);
  }

  automaton determinize() const
  {
    return vcsn::dyn::determinize(aut_);
  }

  automaton difference(const automaton& rhs) const
  {
    return vcsn::dyn::difference(aut_, rhs.aut_);
  }

  polynomial enumerate(unsigned max) const;

  weight eval(const std::string& s) const;

  automaton infiltration(const automaton& rhs) const
  {
    return vcsn::dyn::infiltration(aut_, rhs.aut_);
  }

  bool is_ambiguous() const
  {
    return vcsn::dyn::is_ambiguous(aut_);
  }

  bool is_complete() const
  {
    return vcsn::dyn::is_deterministic(aut_);
  }

  bool is_deterministic() const
  {
    return vcsn::dyn::is_deterministic(aut_);
  }

  bool is_eps_acyclic() const
  {
    return vcsn::dyn::is_eps_acyclic(aut_);
  }

  bool is_equivalent(const automaton& rhs) const
  {
    return vcsn::dyn::are_equivalent(aut_, rhs.aut_);
  }

  bool is_normalized() const
  {
    return vcsn::dyn::is_normalized(aut_);
  }

  bool is_proper() const
  {
    return vcsn::dyn::is_proper(aut_);
  }

  bool is_standard() const
  {
    return vcsn::dyn::is_standard(aut_);
  }

  bool is_trim() const
  {
    return vcsn::dyn::is_standard(aut_);
  }

  bool is_useless() const
  {
    return vcsn::dyn::is_useless(aut_);
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(aut_);
  }

  automaton minimize() const
  {
    return vcsn::dyn::minimize(aut_);
  }

  automaton power(unsigned n) const
  {
    return vcsn::dyn::power(aut_, n);
  }

  std::string format(const std::string& format = "dot") const
  {
    std::ostringstream os;
    vcsn::dyn::print(aut_, os, format);
    return os.str();
  }

  automaton product(const automaton& rhs) const
  {
    return vcsn::dyn::product(aut_, rhs.aut_);
  }

  automaton proper() const
  {
    return vcsn::dyn::proper(aut_);
  }

  ratexp to_ratexp() const;

  polynomial shortest(unsigned max) const;

  automaton shuffle(const automaton& rhs) const
  {
    return vcsn::dyn::shuffle(aut_, rhs.aut_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(aut_);
  }

  automaton star() const
  {
    return vcsn::dyn::star(aut_);
  }

  automaton sum(const automaton& rhs) const
  {
    return vcsn::dyn::sum(aut_, rhs.aut_);
  }

  automaton transpose()
  {
    return vcsn::dyn::transpose(aut_);
  }

  automaton trim() const
  {
    return vcsn::dyn::trim(aut_);
  }

  automaton union_a(const automaton& rhs) const
  {
    return vcsn::dyn::union_a(aut_, rhs.aut_);
  }

  automaton universal() const
  {
    return vcsn::dyn::universal(aut_);
  }

  vcsn::dyn::automaton aut_;
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
    val_ = vcsn::dyn::read_polynomial(is, ctx.ctx_);
  }

  std::string __repr__() const
  {
    return format();
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  vcsn::dyn::polynomial val_;
};


/*---------.
| ratexp.  |
`---------*/

struct ratexp
{
  ratexp(const vcsn::dyn::ratexp& r)
    : r_(r)
  {}

  ratexp(const context& ctx, const std::string& r)
  {
    std::istringstream is(r);
    auto rs = vcsn::dyn::make_ratexpset(ctx.ctx_);
    r_ = vcsn::dyn::read_ratexp(is, rs);
  }

  std::string __repr__() const
  {
    return format();
  }

  std::string _repr_latex_() const
  {
    return format("latex");
  }

  weight constant_term() const;

  polynomial derivation(const std::string& s) const
  {
    return vcsn::dyn::derivation(r_, s);
  }

  automaton derived_term() const
  {
    return vcsn::dyn::derived_term(r_);
  }

  ratexp expand() const
  {
    return vcsn::dyn::expand(r_);
  }

  bool is_equivalent(const ratexp& rhs) const
  {
    return vcsn::dyn::are_equivalent(r_, rhs.r_);
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(r_);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(r_, os, format);
    return os.str();
  }

  polynomial split() const
  {
    return vcsn::dyn::split(r_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(r_);
  }

  ratexp star_normal_form() const
  {
    return vcsn::dyn::star_normal_form(r_);
  }

  automaton thompson() const
  {
    return vcsn::dyn::thompson(r_);
  }

  ratexp transpose()
  {
    return vcsn::dyn::transpose(r_);
  }

  vcsn::dyn::ratexp r_;
};


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
    val_ = vcsn::dyn::read_weight(is, ctx.ctx_);
  }

  std::string __repr__() const
  {
    return format();
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
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

polynomial automaton::enumerate(unsigned max) const
{
  return vcsn::dyn::enumerate(aut_, max);
}

weight automaton::eval(const std::string& s) const
{
  return vcsn::dyn::eval(aut_, s);
}

automaton automaton::ladybird(const context& ctx, unsigned n)
{
  return vcsn::dyn::ladybird(ctx.ctx_, n);
}

polynomial automaton::shortest(unsigned max) const
{
  return vcsn::dyn::shortest(aut_, max);
}

ratexp automaton::to_ratexp() const
{
  return vcsn::dyn::aut_to_exp(aut_);
}


/*------------------------.
| ratexp implementation.  |
`------------------------*/

weight ratexp::constant_term() const
{
  return vcsn::dyn::constant_term(r_);
}


/*--------------.
| vcsn_python.  |
`--------------*/

BOOST_PYTHON_MODULE(vcsn_python)
{
  namespace bp = boost::python;

  bp::class_<automaton>("automaton", bp::init<const ratexp&>())
    .def("ladybird", &automaton::ladybird).staticmethod("ladybird")

    .def("__pow__", &automaton::power)
    .def("__repr__", &automaton::__repr__)
    .def("__sub__", &automaton::difference)

    .def("accessible", &automaton::accessible)
    .def("coaccessible", &automaton::coaccessible)
    .def("complement", &automaton::complement)
    .def("complete", &automaton::complete)
    .def("concatenate", &automaton::concatenate)
    .def("determinize", &automaton::determinize)
    .def("difference", &automaton::difference)
    .def("enumerate", &automaton::enumerate)
    .def("eval", &automaton::eval)
    .def("format", &automaton::format)
    .def("infiltration", &automaton::infiltration)
    .def("is_ambiguous", &automaton::is_ambiguous)
    .def("is_complete", &automaton::is_complete)
    .def("is_deterministic", &automaton::is_deterministic)
    .def("is_eps_acyclic", &automaton::is_eps_acyclic)
    .def("is_equivalent", &automaton::is_equivalent)
    .def("is_proper", &automaton::is_proper)
    .def("is_standard", &automaton::is_standard)
    .def("is_trim", &automaton::is_trim)
    .def("is_useless", &automaton::is_useless)
    .def("is_valid", &automaton::is_valid)
    .def("minimize", &automaton::minimize)
    .def("product", &automaton::product)
    .def("proper", &automaton::proper)
    .def("ratexp", &automaton::to_ratexp, "Conversion to ratexp.")
    .def("shortest", &automaton::shortest)
    .def("shuffle", &automaton::shuffle)
    .def("standard", &automaton::standard)
    .def("star", &automaton::star)
    .def("sum", &automaton::sum)
    .def("transpose", &automaton::transpose)
    .def("trim", &automaton::trim)
    .def("union", &automaton::union_a)
    .def("universal", &automaton::universal)
    ;

  bp::class_<context>("context", bp::init<const std::string&>())
    .def("__repr__", &context::__repr__)
    .def("format", &context::format)
   ;

  bp::class_<ratexp>("ratexp", bp::init<const context&, const std::string&>())
    .def("__repr__", &ratexp::__repr__)
    .def("_repr_latex_", &ratexp::_repr_latex_)
    .def("constant_term", &ratexp::constant_term)
    .def("derivation", &ratexp::derivation)
    .def("derived_term", &ratexp::derived_term)
    .def("expand", &ratexp::expand)
    .def("format", &ratexp::format)
    .def("is_equivalent", &ratexp::is_equivalent)
    .def("is_valid", &ratexp::is_valid)
    .def("split", &ratexp::split)
    .def("standard", &ratexp::standard)
    .def("star_normal_form", &ratexp::star_normal_form)
    .def("thompson", &ratexp::thompson)
    .def("transpose", &ratexp::transpose)
    ;

  bp::class_<polynomial>("polynomial",
                         bp::init<const context&, const std::string&>())
    .def("__repr__", &polynomial::__repr__)
    .def("format", &polynomial::format)
   ;

  bp::class_<weight>("weight", bp::init<const context&, const std::string&>())
    .def("__repr__", &weight::__repr__)
    .def("format", &weight::format)
   ;

}
