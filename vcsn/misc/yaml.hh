#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/any.hpp>

#include <yaml-cpp/yaml.h>

#include <vcsn/misc/export.hh> // LIBVCSN_API
#include <vcsn/misc/raise.hh>

namespace vcsn LIBVCSN_API
{
  namespace detail
  {
    /// The class representing the YAML configuration
    /// (act as the top of the configuration tree).
    class config
    {
      public:
        /// The class returned by [] operators.
        class config_value
        {
          public:
            class BadNode{};
            config_value(YAML::Node n);
            config_value(const config&);
            config_value(const config_value& other);

            /// Assign a new value to this key.
            config_value&
            operator=(config_value rhs);


            template <typename T>
            T as() const
            {
              if (!node_.IsScalar())
                throw new BadNode;
              return node_.as<T>();
            }

            /// Get the node value as a string.
            std::string str() const;

            config_value operator[](const std::string& key) const;
            std::vector<std::string> keys() const;

            /// Check that this node refers to a key that exists in the tree.
            bool is_valid(const std::string& key) const;
            /// Remove a key.
            void remove(const std::string& key);

            using iterator = YAML::Node::iterator;
            iterator begin();
            iterator end();

            /// Merge a value into another one - and modify the first.
            void merge(const config_value& from);

            std::ostream& print(std::ostream& out) const;
            friend void swap(config_value& first, config_value& second);

          private:
            std::unique_ptr<std::vector<std::string>> gen_keys() const;

            mutable std::unique_ptr<const std::vector<std::string>> keys_;
            YAML::Node node_;
        };

        config();
        void load_home_config(const std::string& filename);

        /// Access a subkey.
        config_value operator[](const std::string& key);

      private:

        // Templated because node[] gives us rvalues.
        template <typename T>
        static void merge_recurse(const YAML::Node& from, T&& out);

        YAML::Node config_tree;
    };

    inline
    std::ostream& operator<<(std::ostream& out, const config::config_value& v)
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

  /// Main interface to the configuration function
  /// key it the 'path' to the desired key using periofs
  /// as a separator.
  std::string configuration(const std::string& key);

  namespace detail
  {
    // Templated because node[] gives us rvalues.
    template <typename T>
    void config::merge_recurse(const YAML::Node& from, T&& out)
    {
      if (from.IsScalar())
        out = from;
      else if (from.IsSequence())
        out = from;
      else if (from.IsMap())
      {
        for (auto e : from)
        {
          auto key = e.first.as<std::string>();
          if (!out[key])
          {
            auto node = YAML::Node();
            merge_recurse(e.second, node);
            out[key] = node;
          }
          else if (out.Tag() != "!StyleList")
            merge_recurse(e.second, out[key]);
          else
            raise("bad config value");
        }
      }
    }
  }
}
