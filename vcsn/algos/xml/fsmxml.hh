#ifndef VCSN_ALGOS_XML_FSMXML_HH_
# define VCSN_ALGOS_XML_FSMXML_HH_

# include <vcsn/algos/xml/handlers-base.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/algos/xml/xml-eq.hh>

# define EQUAL(NodeName) eq(eq_.NodeName, localname)
# define ATTR_NAME_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getLocalName(I))
# define ATTR_VALUE_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getValue(I))

namespace vcsn
{
  namespace details
  {
    // FSMXML Root Handler.
    template <typename StructureHandler>
    class FSMXMLHandler : public Handler
    {
    public:
      using content_handler_t = StructureHandler;
      using context_t = typename content_handler_t::context_t;

      FSMXMLHandler(const context_t& ctx, parser_t& parser)
        : Handler(parser)
        , content_(ctx, parser, *this)
      {}

      virtual void start(string, string, string,
                         attributes attr) override final
      {
        // We have only one attribute
        if (attr.getLength() != 1)
          throw
            std::domain_error("Wrong number of attributes for the fsmxml "
                              "node.");
        // Check the version of the root
        if (ATTR_NAME_EQUAL(version, 0))
          {
            if (!ATTR_VALUE_EQUAL(version_number, XMLSize_t()))
              throw std::domain_error(std::string("Bad version number ("
                                                  "excepted 1.1), having ")
                                      + transcode(attr.getValue(XMLSize_t()))
                                      + ".");
          }
        else
          throw std::domain_error(std::string("Bad attribute: ") +
                                  transcode(attr.getLocalName(0)) + '.');
        parser_.setContentHandler(&content_);
      }

      virtual void end(string, string, string) override final
      {}

      rat::exp_t get_parsed_content()
      {
        return content_.get_parsed_content();
      }

    private:
      content_handler_t content_;
    };

  } // namespace details
} // namespace vcsn

# undef ATTR_NAME_EQUAL
# undef ATTR_VALUE_EQUAL
# undef EQUAL

#endif // !VCSN_ALGOS_XML_FSMXML_HH_
