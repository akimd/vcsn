#ifndef VCSN_ALGOS_XML_HH
# define VCSN_ALGOS_XML_HH

# include <sstream>

# include <xercesc/sax2/XMLReaderFactory.hpp>

# include <vcsn/algos/xml/write.hh>

# include <vcsn/algos/xml/error-handlers.hh>
# include <vcsn/algos/xml/ios.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  template <class Context>
  inline
  void
  xml(const Context& ctx, const vcsn::rat::exp_t exp, std::ostream& out)
  {
    XML<Context> x(ctx, exp);
    x.print(out);
  }

  template <class Context>
  inline
  std::string
  xml(const Context& ctx, const rat::exp_t exp)
  {
    std::ostringstream o;
    xml(ctx, exp, o);
    return o.str();
  }


  /*-----------.
  | dyn::xml.  |
  `-----------*/

  namespace dyn
  {
    namespace details
    {
      // ratexp, ostream -> ostream
      template <typename Ctx>
      void
      xml(const dyn::ratexp& exp, std::ostream& out)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp(), out);
      }

      using xml_stream_t =
        auto (const dyn::ratexp& e, std::ostream& out) -> void;

      bool xml_register(const std::string& ctx, const xml_stream_t& fn);


      // ratexp -> string
      template <typename Ctx>
      std::string
      xml(const dyn::ratexp& exp)
      {
        return xml(dynamic_cast<const Ctx&>(exp->ctx()), exp->ratexp());
      }

      using xml_string_t = auto (const dyn::ratexp& e) -> std::string;

      bool xml_register(const std::string& ctx, const xml_string_t& fn);
    } // namespace details
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_HH
