#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/any.hpp>

#include <yaml-cpp/yaml.h>

#include <vcsn/misc/export.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn LIBVCSN_API
{
  namespace detail
  {
    /// A YAML configuration.
    ///
    /// Acts as the top of the configuration tree.
    class config
    {
    public:
      using Node = YAML::Node;

      /// The class returned by [] operators.
      class value
      {
      public:
        class BadNode {};
        value(Node n);
        value(const config&);
        value(const value& other);

        /// Assign a new value to this key.
        value&
        operator=(value rhs);


        template <typename T>
        T as() const
        {
          if (!node_.IsScalar())
            throw BadNode{};
          return node_.as<T>();
        }

        /// Get the node value as a string.
        std::string str() const;

        value operator[](const std::string& key) const;
        std::vector<std::string> keys() const;

        /// Check that this node refers to a key that exists in the tree.
        bool is_valid(const std::string& key) const;
        /// Remove a key.
        void remove(const std::string& key);

        using iterator = Node::iterator;
        iterator begin();
        iterator end();

        /// Merge a value into another one - and modify the first.
        void merge(const value& from);

        std::ostream& print(std::ostream& out) const;
        friend void swap(value& first, value& second);

      private:
        std::unique_ptr<std::vector<std::string>> gen_keys() const;

        mutable std::unique_ptr<const std::vector<std::string>> keys_;
        Node node_;
      };

      /// Load all the configuration files.
      config();

      /// Access a subkey.
      value operator[](const std::string& key);

    private:
      Node config_tree_;
    };

    inline
    std::ostream& operator<<(std::ostream& out, const config::value& v)
    {
      return v.print(out);
    }
  }

  /// Get the configuration singleton.
  inline
  detail::config& get_config()
  {
    static auto conf = detail::config{};
    return conf;
  }

  /// Get the string mapped by key (e.g., "configuration.version",
  /// "dot.styles").
  std::string configuration(const std::string& key);
}
