#ifndef VCSN_ALGOS_XML_IOS_HXX
# define VCSN_ALGOS_XML_IOS_HXX

# include <vcsn/algos/xml/ios.hh>

XERCES_CPP_NAMESPACE_BEGIN

template<class Ostream>
void
XMLPARSER_EXPORT
XMLXercesStream<Ostream>::writeChars(const XMLByte* const toWrite,
                     const XMLSize_t,
                     XMLFormatter* const)
{
  os_ << toWrite;
}

XERCES_CPP_NAMESPACE_END

namespace vcsn
{
  namespace detail
  {
    inline
    XMLSize_t
    BinCxxInputStream::readBytes(XMLByte *const toFill,
                                 const XMLSize_t maxToRead)
    {
      // istream::readsome does not seem to work on SunOS
      unsigned s = in_->rdbuf()->sgetn((char *)toFill, maxToRead);
      pos_ += s;
      return s;
    }

    inline
    XERCES_FILEPOS
    BinCxxInputStream::curPos() const
    {
      return pos_;
    }

    inline
    BinCxxInputStream*
    CxxInputSource::makeStream() const
    {
      return new BinCxxInputStream(in_);
    }

    static inline void
    print_xml(std::ostream& out,
              xercesc::DOMImplementation* impl,
              xercesc::DOMElement* root)
    {
      using namespace xercesc;

      XMLXercesStream<std::ostream>* outstream = new XMLXercesStream<std::ostream>(out);

# if XERCES_VERSION_MAJOR == 2
      DOMWriter* writer = impl->createDOMWriter();

      if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      writer->writeNode(outstream, *root);
# elif XERCES_VERSION_MAJOR > 2
      DOMLSSerializer* writer = impl->createLSSerializer();
      DOMLSOutput *target = impl->createLSOutput();
      target->setByteStream(outstream);
      DOMConfiguration *config = writer->getDomConfig();

      if (config->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        config->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      writer->write(root, target);
# endif

      writer->release();

      delete outstream;
    }

  } // namespace detail

} // namespace vcsn

#endif // !VCSN_ALGOS_XML_IOS_HXX
