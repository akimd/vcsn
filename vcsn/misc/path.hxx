/**
 ** \file vcsn/misc/path.hxx
 ** \brief Inline implementation of vcsn::path.
 */

#ifndef LIBPORT_PATH_HXX
# define LIBPORT_PATH_HXX

# include <sys/stat.h>
# include <vcsn/misc/path.hh>

namespace vcsn
{

  /*---------------------.
  | Path::invalid_path.  |
  `---------------------*/

  inline
  path::invalid_path::invalid_path(const std::string& msg)
    : msg_(msg)
  {}

  inline
  const char*
  path::invalid_path::what() const throw ()
  {
    return msg_.c_str();
  }

  inline
  path::invalid_path::~invalid_path() throw ()
  {}


  /*-------.
  | path.  |
  `-------*/

  inline
  std::ostream&
  operator<<(std::ostream& o, const path& p)
  {
    return p.dump(o);
  }

  inline
  bool path::is_absolute() const
  {
    return value_[0] == '/';
  }

  inline
  const char*
  path::c_str() const
  {
    return value_.c_str();
  }

  inline
  const std::string&
  path::string() const
  {
    return value_;
  }

  inline
  const path::value_type&
  path::value_get() const
  {
    return value_;
  }

  inline
  path::value_type&
  path::value_get()
  {
    return value_;
  }
}

#endif // !LIBPORT_PATH_HXX
