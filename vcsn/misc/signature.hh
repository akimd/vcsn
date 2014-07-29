#ifndef VCSN_MISC_SIGNATURE_HH
# define VCSN_MISC_SIGNATURE_HH

# include <iosfwd>
# include <string>
# include <vector>

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/export.hh>

namespace vcsn LIBVCSN_API
{
  /// Signature of a function call.
  struct signature
  {
    using sig_t = std::vector<std::string>;

    signature() = default;

    signature(const std::initializer_list<std::string>& l)
      : sig(l)
    {}

    std::string to_string() const;

    ATTRIBUTE_PURE
    bool operator<(const signature& that) const
    {
      return sig < that.sig;
    }

    sig_t::const_iterator begin() const
    {
      return std::begin(sig);
    }

    sig_t::const_iterator end() const
    {
      return std::end(sig);
    }

    sig_t sig;
  };

  /// Output a string, escaping special characters.
  std::ostream& operator<<(std::ostream& os, const signature& sig);
}

#endif // !VCSN_MISC_SIGNATURE_HH
