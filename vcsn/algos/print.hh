#ifndef VCSN_ALGOS_PRINT_HH
# define VCSN_ALGOS_PRINT_HH

# include <iosfwd>

#include <boost/algorithm/string.hpp>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/nullableset.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/labelset/wordset.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/weights/b.hh>
# include <vcsn/weights/z.hh>
# include <vcsn/weights/q.hh>
# include <vcsn/weights/r.hh>
# include <vcsn/weights/f2.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{

  std::ostream&
  print_ls(const ctx::oneset& ls,
           std::ostream& o, const std::string& format);

  template <typename LabelSet, typename WeightSet>
  inline
  std::ostream&
  print_ctx(const ctx::context<LabelSet, WeightSet>& ctx,
            std::ostream& o, const std::string& format)
  {
    print_ls(*ctx.labelset(), o, format);
    if (format == "latex")
      o << "\\rightarrow";
    else
      o << '_';
    return print_ws(*ctx.weightset(), o, format);
  }

  inline
  std::ostream&
  print_ws(const b& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{" << boost::to_upper_copy(ws.vname()) << '}';
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ws(const z& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{" << boost::to_upper_copy(ws.vname()) << '}';
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ws(const q& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{" << boost::to_upper_copy(ws.vname()) << '}';
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ws(const r& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{" << boost::to_upper_copy(ws.vname()) << '}';
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ws(const f2& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{F}_2";
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ws(const zmin& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "\\mathbb{Z}_{min}";
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  template <typename Context>
  inline
  std::ostream&
  print_ws(const ratexpset<Context>& ws,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	print_ws(*ws.weightset(), o, format);
	o << "\\,\\mathsf{RatE}\\,";
	print_ls(*ws.labelset(), o, format);
      }
    else if (format == "text")
      o << ws.vname();
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  template <typename L>
  inline
  std::ostream&
  print_ls(const set_alphabet<L>& alphabet,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	const char *sep = "\\{";
	for (auto c: alphabet)
	  {
	    o << sep << c;
	    sep = ", ";
	  }
	o << "\\}";
      }
    else if (format == "text")
      o << alphabet.vname(true);
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  template <typename GenSet>
  inline
  std::ostream&
  print_ls(const ctx::letterset<GenSet>& ls,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      print_ls(*ls.genset(), o, format);
    else if (format == "text")
      o << ls.vname(true);
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  template <typename GenSet>
  inline
  std::ostream&
  print_ls(const ctx::nullableset<GenSet>& ls,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	print_ls(*ls.genset(), o, format);
	o << "^?";
      }
    else if (format == "text")
      o << ls.vname(true);
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  inline
  std::ostream&
  print_ls(const ctx::oneset& ls,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      o << "1";
    else if (format == "text")
      o << ls.vname(true);
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  template <typename GenSet>
  inline
  std::ostream&
  print_ls(const ctx::wordset<GenSet>& ls,
           std::ostream& o, const std::string& format)
  {
    if (format == "latex")
      {
	print_ls(*ls.genset(), o, format);
	o << "^*";
      }
    else if (format == "text")
      o << ls.vname(true);
    else
      throw std::runtime_error("invalid format: " + format);
    return o;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Ostream, typename String>
      std::ostream& print_ctx(const context& ctx, std::ostream& o,
			      const std::string& format)
      {
        const auto& c = ctx->as<Context>();
        return vcsn::print_ctx<typename Context::labelset_t,
                               typename Context::weightset_t>(c, o, format);
      }

      REGISTER_DECLARE(print_ctx,
                       (const context& c, std::ostream& o,
                        const std::string& format) -> std::ostream&);
    }
  }

  /*---------------------------------.
  | list/print(polynomial, stream).  |
  `---------------------------------*/

  template <typename PolynomialSet>
  inline
  std::ostream&
  list(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
       std::ostream& o)
  {
    bool first = true;
    for (const auto& m: p)
      {
        if (!first)
          o << std::endl;
        first = false;
        ps.print(o, m);
      }
    return o;
  }

  template <typename PolynomialSet>
  inline
  std::ostream&
  print(const PolynomialSet& ps, const typename PolynomialSet::value_t& p,
        std::ostream& o)
  {
    return ps.print(o, p);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSet, typename Ostream>
      std::ostream& list_polynomial(const polynomial& polynomial,
                                    std::ostream& o)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::list<PolynomialSet>(p.get_polynomialset(),
                                         p.polynomial(), o);
      }

      REGISTER_DECLARE(list_polynomial,
                       (const polynomial& p, std::ostream& o) -> std::ostream&);

      /// Bridge.
      template <typename PolynomialSet, typename Ostream>
      std::ostream& print_polynomial(const polynomial& polynomial,
                                     std::ostream& o)
      {
        const auto& p = polynomial->as<PolynomialSet>();
        return vcsn::print<PolynomialSet>(p.get_polynomialset(),
                                          p.polynomial(), o);
      }

      REGISTER_DECLARE(print_polynomial,
                       (const polynomial& p, std::ostream& o) -> std::ostream&);
    }
  }


  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

  template <typename RatExpSet>
  inline
  std::ostream&
  print(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
        std::ostream& o, const std::string& format)
  {
    return rs.print(o, e, format);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Ostream, typename String>
      std::ostream& print_exp(const ratexp& exp, std::ostream& o,
			      const std::string& format)
      {
        const auto& e = exp->as<RatExpSet>();
        return vcsn::print(e.get_ratexpset(), e.ratexp(), o, format);
      }

      REGISTER_DECLARE(print_exp,
                       (const ratexp& aut, std::ostream& o,
			const std::string& format) -> std::ostream&);
    }
  }

  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

#if 0
  template <typename WeightSet>
  inline
  std::ostream&
  print(const WeightSet& ws, const typename WeightSet::value_t& w,
        std::ostream& o)
  {
    return ws.print(o, w);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSet, typename Ostream>
      std::ostream& print_weight(const weight& weight, std::ostream& o)
      {
        const auto& w = weight->as<WeightSet>();
        return vcsn::print<WeightSet>(w.get_weightset(), w.weight(), o);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o) -> std::ostream&);
    }
  }

}

#endif // !VCSN_ALGOS_PRINT_HH
