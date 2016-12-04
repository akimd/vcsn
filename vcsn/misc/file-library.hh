/**
 ** \file vcsn/misc/file-library.hh
 ** \brief Manage sets of inclusion paths.
 */

#pragma once

#include <stdexcept>
#include <list>
#include <string>

#include <boost/filesystem/path.hpp>

#include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /** \brief Manage search paths.

      Store a search path and all informations used for handling
      paths when processing import directives. */
  class file_library
  {
  public:
    using self_t = file_library;

    using path = boost::filesystem::path;
    /// Yes, a list is really handy here, for push/pop front.
    using path_list_type = std::list<path>;

    /// Exception thrown when a file cannot be located.
    class LIBVCSN_API not_found : public std::runtime_error
    {
    public:
      not_found(const path& file);
    };

    /// \name Constructor.
    /// \{
    /// An empty library.
    file_library();
    /// Init the library with one path.
    file_library(const path& p);
    /// Init the library with a string to split at \a separator.
    file_library(const std::string& library, const char* separator);

    /// Handle a list of search-paths.
    /// \param r  the collection of search-paths.
    ///           each occurrence of an additional colon marks the
    ///           place where the following search-paths will be inserted.
    ///           If there is none, the remaining components are ignored.
    /// \param sep  the separator to split search-paths (typically colon).
    template <typename ForwardRange>
      file_library(const ForwardRange& r, const char* sep);
    /// \}

    /// \name Managing inclusion paths.
    /// \{
    self_t& push_back(const path& p);
    /// Split \a library on \a separator and put at the end of path list.
    self_t& push_back(const std::string& library, const char* separator);

    /// Handle a list of search-paths.
    /// \param r  the collection of search-paths.
    ///           each occurrence of an additional colon marks the
    ///           place where the following search-paths will be inserted.
    ///           If there is none, the remaining components are ignored.
    /// \param sep  the separator to split search-paths (typically colon).
    template <typename ForwardRange>
      self_t& push_back(const ForwardRange& r, const char* sep);

    template <typename Iterable>
      self_t& push_back(const std::string& user_path,
                           const Iterable& default_path,
                           const char* sep);

    self_t& append(const path& p);

    self_t& push_front(const path& p);

    /// Split \a library on \a separator and put at the beginning of path list.
    /// \warning "a:b:c" will end with "c" first, then "b", then "a",
    /// then what was already here.
    self_t& push_front(const std::string& library, const char* separator);
    self_t& prepend(const path& p);
    /// \}

    /// \name current directory.
    /// \{
    void push_current_directory(const path& p);
    void pop_current_directory();
    path current_directory_get() const;
    /// \}


    using strings_type = std::vector<std::string>;
    /// Split \a lib at each occurrence of \a sep, return the list
    /// of components.
    /// Behaves especially on Windows when splitting on ":", in
    /// order to preserve drive prefixes (i.e., "c:foo:d:bar" is
    /// split in "c:foo", "d:bar").
    static strings_type split(const std::string& lib, const char* sep);


    /// \name Finding files.
    /// \{
    /** \brief Search a file.
     *
     * Determine real path of \a file, by looking in search path if
     * necessary.
     *
     * \return Path for \a file.
     * \throw  not_found    file cannot be found,
     *                      in which case errno is set to ENOENT. */
    path find_file(const path& file) const;
    /// \}

    /// \name Printing.
    /// \{
    std::ostream& dump(std::ostream& ostr) const;
    /// \}

    /// \name Accessor.
    /// \{
    const path_list_type& search_path_get() const;
    path_list_type& search_path();
    /// \}

  private:
    /// Push the working directory on the stack.
    void push_cwd();

    /** \brief Find file \a filename using include path.
        \return Absolute path where the file lies.
        \throw  not_found   file not found. */
    path find_in_search_path(const path& filename) const;

    /// Inclusion path list.
    path_list_type search_path_;

    /// Current directory stack.
    path_list_type current_directory_;
  };

  /// Print \a l on \a o.
  std::ostream& operator<<(std::ostream& o, const file_library& l);

}

#include <vcsn/misc/file-library.hxx>
