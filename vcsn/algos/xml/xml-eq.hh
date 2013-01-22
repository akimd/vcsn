#ifndef VCSN_ALGOS_XML_XML_EQ_HH
# define VCSN_ALGOS_XML_XML_EQ_HH

# include <xercesc/util/XMLString.hpp>

namespace vcsn
{
  namespace details
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
        : atom(TRANS("atom"))
        , enumeration(TRANS("enum"))
        , free(TRANS("free"))
        , genDescript(TRANS("genDescript"))
        , genKind(TRANS("genKind"))
        , genSort(TRANS("genSort"))
        , label(TRANS("label"))
        , letter(TRANS("letter"))
        , numerical(TRANS("numerical"))
        , one(TRANS("one"))
        , operations(TRANS("operations"))
        , plus(TRANS("plus"))
        , product(TRANS("prod"))
        , series(TRANS("series"))
        , set(TRANS("set"))
        , simple(TRANS("simple"))
        , star(TRANS("star"))
        , type(TRANS("type"))
        , value(TRANS("value"))
        , valueType(TRANS("valueType"))
        , valueSet(TRANS("valueSet"))
        , valueSetElt(TRANS("valueSetElt"))
        , version(TRANS("version"))
        , version_number(TRANS("1.1"))
        , weight(TRANS("weight"))
        , weightSet(TRANS("weightSet"))
        , zero(TRANS("zero"))
      {
      }
# undef TRANS
# define RELEASE(Type) xercesc::XMLString::release(&Type)
      ~xmlEq()
      {
        RELEASE(atom);
        RELEASE(enumeration);
        RELEASE(free);
        RELEASE(genDescript);
        RELEASE(genKind);
        RELEASE(genSort);
        RELEASE(label);
        RELEASE(letter);
        RELEASE(numerical);
        RELEASE(one);
        RELEASE(operations);
        RELEASE(plus);
        RELEASE(product);
        RELEASE(series);
        RELEASE(set);
        RELEASE(simple);
        RELEASE(star);
        RELEASE(type);
        RELEASE(value);
        RELEASE(valueType);
        RELEASE(valueSet);
        RELEASE(valueSetElt);
        RELEASE(version);
        RELEASE(version_number);
        RELEASE(weight);
        RELEASE(weightSet);
        RELEASE(zero);
      }
# undef RELEASE
      // Rational expression tokens.
      string atom;
      string enumeration;
      string free;
      string genDescript;
      string genKind;
      string genSort;
      string label;
      string letter;
      string numerical;
      string one;
      string operations;
      string plus;
      string product;
      string series;
      string set;
      string simple;
      string star;
      string type;
      string value;
      string valueType;
      string valueSet;
      string valueSetElt;
      string version;
      string version_number;
      string weight;
      string weightSet;
      string zero;
    };


  } // namespace details
} // namespace vcsn

# undef TRANS

#endif // !VCSN_ALGOS_XML_XML_EQ_HH
