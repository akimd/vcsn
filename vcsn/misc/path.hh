/**
 ** \file vcsn/misc/path.hh
 ** \brief Declaration of vcsn::path.
 */

#ifndef VCSN_MISC_PATH_HH
# define VCSN_MISC_PATH_HH

# include <string>
# include <list>

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  /** \brief Paths in filesystems, i.e., file names.
   **
   ** We should rather use boost::filesystem::path, but as of today
   ** it does not work on Mac OS X: https://trac.macports.org/ticket/41588.
   **/
  class LIBVCSN_API path
  {
  public:
    /// Exception thrown on invalid path
    class LIBVCSN_API invalid_path : public std::exception
    {
    public:
      invalid_path(const std::string& msg);
      const char* what() const throw ();
      virtual ~invalid_path() throw ();
    private:
      std::string msg_;
    };

    /// The "native" type we wrap.
    using value_type = std::string;

    /// \name Constructors.
    /// \{

    /// Init object with \a path.
    /** @throw invalid_path if \a p isn't a valid path
     */
    path(const std::string& p);

    /** @throw invalid_path if \a p isn't a valid path
     */
    path(const char* p);
    /// \}

    /// \name Operations on path.
    /// \{
    /** @throw invalid_path if \a rhs is absolute.
     */
    path& operator/=(const path& rhs);
    /** @throw invalid_path if \a rhs is absolute.
     */
    path operator/(const path& rhs) const;
    bool operator==(const path& rhs) const ATTRIBUTE_PURE;

    std::string basename() const;
    /// The extension of the file, or "" if not applicable.
    std::string extension() const;
    /// \}

    /// Return the current working directory.
    static path cwd();

    /// Whether starts with /.
    bool is_absolute() const;

    /// Return a path which is the parent directory.
    /// If path is relative, current directory is
    /// returned when asking for parent of 1-depth entity.
    path parent_path() const;

    /// Also known as basename.
    path filename() const;

    /// \name Printing and converting.
    /// \{
    /// path is represented with a list of directories.
    using path_type = std::list<std::string>;
    const std::string& string() const;
    const char* c_str() const;
    path_type components() const;
    std::ostream& dump(std::ostream& ostr) const;
    /// \}

    const value_type& value_get() const;
    value_type& value_get();

  private:
    /// Wrapped value.
    value_type value_;

    /// Path separator.
    static const char separator_ = '/';
  };

  LIBVCSN_API
  path absolute(const path& p);

  LIBVCSN_API
  bool exists(const path& p);


  /// Dump \a p on \a o.
  std::ostream& operator<<(std::ostream& o, const path& p);
}

# include <vcsn/misc/path.hxx>

#endif // !VCSN_MISC_PATH_HH
