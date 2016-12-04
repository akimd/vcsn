/**
 ** \file vcsn/misc/file-library.hxx
 ** \brief Implement inline functions of vcsn/misc/file-library.hh
 */

#include <iterator>

namespace vcsn
{

  template <typename ForwardRange>
  file_library::file_library(const ForwardRange& r, const char* sep)
  {
    push_cwd();
    push_back(r, sep);
  }

  template <typename ForwardRange>
  file_library&
  file_library::push_back(const ForwardRange& r, const char* sep)
  {
    bool inserted = false;
    auto first = std::begin(r);
    if (first != std::end(r))
    {
      if (first->empty())
        // Insert the following search path component.
        push_back(skip_first(r), sep);
      else
        for (const std::string& s: split(*first, sep))
        {
          if (!s.empty())
            push_back(s);
          else if (!inserted)
          {
            // Insert the following search path component.
            push_back(skip_first(r), sep);
            inserted = true;
          }
        }
    }
    return *this;
  }

  template <typename Iterable>
  file_library&
  file_library::push_back(const std::string& user_path,
                          const Iterable& default_path,
                          const char* sep)
  {
    bool inserted = false;
    if (user_path.empty())
    {
      // Insert the following search path component.
      for (const std::string& s: default_path)
        if (!s.empty())
          push_back(s);
    }
    else
    {
      for (const std::string& s1: split(user_path, sep))
      {
        if (!s1.empty())
          push_back(s1);
        else if (!inserted)
        {
          for (const std::string& s2: default_path)
            if (!s2.empty())
              push_back(s2);
          inserted = true;
        }
      }
    }
    return *this;
  }

  inline file_library&
  file_library::append(const path& p)
  {
    return push_back(p);
  }

  inline file_library&
  file_library::prepend(const path& p)
  {
    return push_front(p);
  }

  inline const file_library::path_list_type&
  file_library::search_path_get() const
  {
    return search_path_;
  }

  inline file_library::path_list_type&
  file_library::search_path()
  {
    return search_path_;
  }

  inline std::ostream&
  operator<<(std::ostream& ostr, const file_library& l)
  {
    return l.dump(ostr);
  }
}
