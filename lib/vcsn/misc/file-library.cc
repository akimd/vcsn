/**
 ** \file lib/vcsn/misc/file-library.cc
 ** \brief Implements vcsn::file_library.
 */

#include <vcsn/misc/file-library.hh>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <vcsn/misc/escape.hh>

namespace vcsn
{

  file_library::not_found::not_found(const path& p)
    : std::runtime_error("file not found: " + p.string())
  {}

  void
  file_library::push_cwd()
  {
    push_current_directory(boost::filesystem::current_path());
  }


  file_library::file_library()
  {
    push_cwd();
  }

  file_library::file_library(const path& p)
  {
    push_cwd();
    // Then only process given path.
    push_back(p);
  }


  file_library::file_library(const std::string& lib, const char* sep)
  {
    push_cwd();
    push_back(lib, sep);
  }

  auto
  file_library::split(const std::string& lib, const char* sep)
    -> strings_type
  {
    using tokenizer = boost::tokenizer<boost::char_separator<char>>;
    boost::char_separator<char> seps(sep, "", boost::keep_empty_tokens);
    tokenizer dirs(lib, seps);
    strings_type res;
    for (const std::string& s: dirs)
      res.emplace_back(s);
    return res;
  }

  file_library&
  file_library::push_back(const path& p)
  {
    search_path_.emplace_back(absolute(p));
    return *this;
  }

  file_library&
  file_library::push_back(const std::string& lib, const char* sep)
  {
    for (const std::string& s: split(lib, sep))
      if (!s.empty())
        push_back(s);
    return *this;
  }

  file_library&
  file_library::push_front(const path& p)
  {
    search_path_.emplace_front(absolute(p));
    return *this;
  }

  file_library&
  file_library::push_front(const std::string& lib, const char* sep)
  {
    for (const std::string& s: split(lib, sep))
      if (!s.empty())
        push_front(s);
    return *this;
  }

  void
  file_library::push_current_directory(const path& p)
  {
    current_directory_.push_front(absolute(p));
  }

  void
  file_library::pop_current_directory()
  {
    assert(!current_directory_.empty());
    current_directory_.pop_front();
  }

  auto
  file_library::current_directory_get() const
    -> path
  {
    assert(!current_directory_.empty());
    return *current_directory_.begin();
  }

  auto
  file_library::find_file(const path& file) const
    -> path
  {
    if (file.is_absolute())
    {
      // If file is absolute, just check that it exists.
      if (!exists(file))
      {
        errno = ENOENT;
        throw not_found(file);
      }
      else
        return file;
    }
    // In the current directory?
    else if (exists(current_directory_get() / file))
      return current_directory_get() / file;
    else
      return find_in_search_path(file);
  }

  auto
  file_library::find_in_search_path(const path& filename) const
    -> path
  {
    // Otherwise start scanning the search path.
    for (path dir: search_path_)
      {
        path p = dir / filename;
        if (exists(p))
          return p;
      }

    // File not found in search path.
    errno = ENOENT;
    throw not_found(filename);
  }

  std::ostream&
  file_library::dump(std::ostream& ostr) const
  {
    ostr << ".";
    for (const path& p: search_path_)
      ostr << ":" << p;
    return ostr;
  }
}
