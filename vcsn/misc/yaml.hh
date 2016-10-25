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
  /// The class representing the YAML configuration
  class config
  {
    public:
      class config_value
      {
        public:
          class BadNode{};
          config_value(YAML::Node n);
          config_value(const config_value& other);

          config_value&
          operator=(config_value rhs);

          template <typename T>
          T as() const
          {
            if (!node_.IsScalar())
              throw new BadNode;
            return node_.as<T>();
          }


          std::string str() const;

          config_value operator[](const std::string& key) const;
          std::vector<std::string> keys() const;


          bool is_valid(const std::string& key) const;
          void remove(const std::string& key);

          using iterator = YAML::Node::iterator;
          iterator begin();
          iterator end();

          void merge(const config_value& from);

          std::ostream& print(std::ostream& out) const;
          friend void swap(config_value& first, config_value& second);

        private:
          std::unique_ptr<std::vector<std::string>> gen_keys() const;

          mutable std::unique_ptr<const std::vector<std::string>> keys_;
          YAML::Node node_;
      };

      config(const char* dir_path);
      void load_home_config(const std::string& filename);
      config_value operator[](const std::string& key);

    private:

      // Templated because node[] gives us rvalues
      template <typename T>
      static void merge_recurse(const YAML::Node& from, T&& out);

      YAML::Node config_tree;

  };

  inline
  std::ostream& operator<<(std::ostream& out, const config::config_value& v)
  {
    return v.print(out);
  }

  inline
  config& get_config()
  {
    static auto conf = config{std::getenv("VCSN_DATADIR")};
    return conf;
  }

  // Templated because node[] gives us rvalues
  template <typename T>
  void config::merge_recurse(const YAML::Node& from, T&& out)
  {
    if (from.IsScalar())
      out = from;
    else if (from.IsSequence())
      out = from;
    else if (from.IsMap())
    {
      for(auto e : from)
      {
        auto key = e.first.as<std::string>();
        if(!out[key])
        {
          auto node = YAML::Node();
          merge_recurse(e.second, node);
          out[key] = node;
        }
        else if(out.Tag() != "!StyleList")
          merge_recurse(e.second, out[key]);
        else
          raise("bad config value");
      }
    }
  }
}
