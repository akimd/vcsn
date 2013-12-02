/**
 ** \file lib/vcsn/misc/path.cc
 ** \brief Implement vcsn::path.
 */

#include <cctype>
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h> // MAXPATHLEN
#include <unistd.h>

#include <boost/tokenizer.hpp>

#include <vcsn/misc/path.hh>

namespace vcsn
{
  path::path(const std::string& p)
    : value_(p)
  {}

  path::path(const char* p)
    : path(std::string(p))
  {}

  path
  path::cwd()
  {
    // Store the working directory
    char res[MAXPATHLEN + 1];

    if (!getcwd(res, MAXPATHLEN + 1))
      throw std::runtime_error("working directory name too long");

    return {res};
  }

  path&
  path::operator/=(const path& rhs)
  {
    //    if (rhs.absolute_get())
    //      throw invalid_path(
    //        "Rhs of concatenation is absolute: " + rhs.string());
    value_ += "/" + rhs.value_;
    return *this;
  }

  path
  path::operator/(const path& rhs) const
  {
    path res = *this;
    return res /= rhs;
  }

  bool
  path::operator==(const path& rhs) const
  {
    return value_ == rhs.value_;
  }

  std::ostream&
  path::dump(std::ostream& o) const
  {
    return o << string();
  }

  path::path_type
  path::components() const
  {
    using tokenizer = boost::tokenizer<boost::char_separator<char>>;
    boost::char_separator<char> seps("/", "", boost::keep_empty_tokens);
    tokenizer dirs(value_, seps);

    path_type res;
    for (const std::string& s: dirs)
      res.emplace_back(s);
    return res;
  }

  path path::parent_path() const
  {
    auto slash = value_.rfind('/');
    if (slash == value_.npos || slash == 0)
      return ".";
    else
      return {value_.substr(0, slash - 1)};
  }

  path path::filename() const
  {
    auto slash = value_.rfind('/');
    return {value_.substr(slash + 1)};
  }

  bool exists(const path& p)
  {
    struct stat buf;
    return 0 == stat (p.c_str(), &buf);
  }

  path absolute(const path& p)
  {
    if (p.is_absolute())
      return p;
    else
      return path::cwd() / p;
  }

}
