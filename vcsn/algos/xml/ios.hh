// ios.hh: this file is part of the Vaucanson project.
//
// Vaucanson, a generic library for finite state machines.
//
// Copyright (C) 2004, 2005, 2006, 2008, 2009, 2012 The Vaucanson Group.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// The complete GNU General Public Licence Notice can be found as the
// `COPYING' file in the root directory.
//
// The Vaucanson Group consists of people listed in the `AUTHORS' file.
//

#ifndef VCSN_ALGOS_XML_IOS_HH
# define VCSN_ALGOS_XML_IOS_HH

/**
 * @file vcsn/algos/xml/ios.hh
 *
 * Functions required by the Xerces-C parser to manage streams.
 *
 */

# include <iostream>

# include <xercesc/dom/DOM.hpp>
# include <xercesc/util/BinInputStream.hpp>
# include <xercesc/sax/InputSource.hpp>
# include <xercesc/framework/XMLFormatter.hpp>

/// The type used by Xerces-C++ for the file position type.
# if XERXES_VERSION_MAJOR == 2
#  define XMLSize_t unsigned int
#  define XERCES_FILEPOS XMLSize_t
# elif XERCES_VERSION_MAJOR > 2
#  define XERCES_FILEPOS XMLFilePos
# else
#  error Unsupported Xerces-C++ major version (too old).
# endif

XERCES_CPP_NAMESPACE_BEGIN

/**
 * Provide a streaming support for the Xerces DomWriter.
 * Follow Xerces Specifications.
 *
 * @author Florian Lesaint <florian.lesaint@lrde.epita.fr>
 */

template<class Ostream>
class XMLPARSER_EXPORT XMLXercesStream : public XMLFormatTarget
{
public:

  /**
   * @name Constructor and destructor
   */
  //@{
  XMLXercesStream(std::ostream& os) : os_(os) {};
  ~XMLXercesStream() {};
  //@}

  /**
   * @brief Implementations of the format target interface
   */
  virtual void writeChars(const XMLByte* const toWrite,
                          const XMLSize_t,
                          XMLFormatter* const);

  virtual void flush() {};

private:
  /**
   * @brief Unimplemented methods (should never be used)
   */
  XMLXercesStream(const XMLXercesStream&);
  XMLXercesStream& operator=(const XMLXercesStream&);

  Ostream&    os_;
};

XERCES_CPP_NAMESPACE_END

namespace vcsn
{
  namespace details
  {
    class BinCxxInputStream : public xercesc::BinInputStream
    {
    public:
      BinCxxInputStream(std::istream* in) : in_(in), pos_(0) { }
      virtual XERCES_FILEPOS curPos() const;
      virtual XMLSize_t readBytes(XMLByte *const toFill,
                                  const XMLSize_t maxToRead);
      virtual const XMLCh* getContentType () const { return nullptr; }

    private:
      std::istream* in_;
      unsigned int pos_;
    };

    class CxxInputSource : public xercesc::InputSource
    {
    public:
      CxxInputSource(std::istream* in) : InputSource(), in_(in) {}
      virtual BinCxxInputStream* makeStream() const;

    private:
      std::istream* in_;
    };

  } // namespace details

} // namespace vcsn

# include <vcsn/algos/xml/ios.hxx>

#endif // !VCSN_ALGOS_XML_IOS_HH
