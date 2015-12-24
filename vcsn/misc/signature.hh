#pragma once

#include <iosfwd>
#include <string>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/vector.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn LIBVCSN_API
{
  /// Signature of a function call.
  struct signature
  {
    using sig_t = std::vector<symbol>;

    signature() = default;

    signature(std::initializer_list<symbol> l)
      : sig(l)
    {}

    std::string to_string() const;

    ATTRIBUTE_PURE
    bool operator<(const signature& that) const
    {
      return sig < that.sig;
    }

    ATTRIBUTE_PURE
    bool operator==(const signature& that) const
    {
      return sig == that.sig;
    }

    sig_t::const_iterator begin() const
    {
      return std::begin(sig);
    }

    sig_t::const_iterator end() const
    {
      return std::end(sig);
    }

    symbol& operator[](const size_t n)
    {
      return sig[n];
    }

    const symbol operator[](const size_t n) const
    {
      return sig.at(n);
    }

    size_t size() const
    {
      return sig.size();
    }

    sig_t sig;
  };

  /// Output a string, escaping special characters.
  std::ostream& operator<<(std::ostream& os, const signature& sig);
}

namespace std
{
  template <>
  struct hash<vcsn::signature>
  {
    size_t operator()(const vcsn::signature& sig) const
    {
      return vcsn::hash_value(sig.sig);
    }
  };
}
