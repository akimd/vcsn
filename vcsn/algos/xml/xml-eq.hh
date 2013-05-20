#ifndef VCSN_ALGOS_XML_XML_EQ_HH
# define VCSN_ALGOS_XML_XML_EQ_HH

namespace vcsn
{
  namespace detail
  {

    class xmlEq
    {
      /*
       * Help to improve performances on comparison.
       *
       * Since transcode(std::string) is more expensive than
       * transcode(XMLCh).  We transcode and store all the tokens we
       * might need to compare while parsing a document.
       *
       */

    public:
      using string = XMLCh*;

# define TRANS(STRING) xercesc::XMLString::transcode(STRING)
      xmlEq()
        : label(TRANS("label"))
        , sum(TRANS("sum"))
        , product(TRANS("product"))
        , star(TRANS("star"))
        , one(TRANS("one"))
        , zero(TRANS("zero"))
        , monElmt(TRANS("monElmt"))
        , weight(TRANS("weight"))
      {}

# undef TRANS
# define RELEASE(Type) xercesc::XMLString::release(&Type)

      ~xmlEq()
      {
        RELEASE(label);
        RELEASE(sum);
        RELEASE(product);
        RELEASE(star);
        RELEASE(one);
        RELEASE(zero);
        RELEASE(monElmt);
        RELEASE(weight);
      }

# undef RELEASE

      // Rational expression tokens.
      string label;
      string sum;
      string product;
      string star;
      string one;
      string zero;
      string monElmt;
      string weight;
    };


  } // namespace detail
} // namespace vcsn

# undef TRANS

#endif // !VCSN_ALGOS_XML_XML_EQ_HH
