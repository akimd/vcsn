#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <vcsn/config.hh> // VCSN_DATADIR
#include <vcsn/misc/file-library.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/misc/configuration.hh>

namespace vcsn
{
  namespace detail
  {
    namespace
    {
      config::Node load_file(const boost::filesystem::path& p)
      {
        require(boost::filesystem::exists(p),
                "config file does not exist:", p);
        return YAML::LoadFile(p.string());
      }

      // Templated because node[] gives us rvalues.
      template <typename T>
      void merge_recurse(const config::Node& from, T&& out)
      {
        if (from.IsScalar() || from.IsSequence())
          out = from;
        else if (from.IsMap())
          for (auto e : from)
          {
            auto key = e.first.as<std::string>();
            if (!out[key])
            {
              auto node = config::Node{};
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

    config::value::value(Node n)
      : node_{n}
    {}

    config::value::value(const value& other)
      : node_{other.node_}
    {}

    config::value::value(const config& c)
        : node_{c.config_tree_}
      {}

    config::value&
    config::value::operator=(value rhs)
    {
      swap(*this, rhs);
      return *this;
    }

    std::string config::value::str() const
    {
      return as<std::string>();
    }

    config::value
    config::value::operator[](const std::string& key) const
    {
      require(node_.IsMap(),
              "configuration: requesting a key (", key,
              ") in a leaf");
      require(node_[key].IsDefined(),
              "configuration: invalid key: ", key);
      return value(node_[key]);
    }

    std::vector<std::string> config::value::keys() const
    {
      // We generate the key only once for each value
      if (!keys_)
        keys_ = gen_keys();
      return *keys_;
    }

    bool config::value::is_valid(const std::string& key) const
    {
      return node_[key].IsDefined();
    }

    void config::value::remove(const std::string& key)
    {
#if VCSN_YAML_CPP_REMOVE_WORKS
      node_.remove(key);
#else
      raise("configuration: libyaml-cpp is broken, cannot remove node ", key);
#endif
    }

    auto config::value::begin() -> iterator
    {
      require(node_.IsSequence(), "configuration: node is not a sequence");
      return node_.begin();
    }

    auto config::value::end() -> iterator
    {
      return node_.end();
    }

    void config::value::merge(const value& from)
    {
      auto dest_node = Clone(from.node_);
      merge_recurse(node_, dest_node);
      node_ = dest_node;
    }

    std::ostream& config::value::print(std::ostream& out) const
    {
      return out << node_;
    }

    std::unique_ptr<std::vector<std::string>>
    config::value::gen_keys() const
    {
      auto res = std::make_unique<std::vector<std::string>>();
      require(node_.IsMap(), "configuration: node is not a map");

      for (auto e : node_)
        res->emplace_back(e.first.as<std::string>());

      // We must sort the keys to have a deterministic output
      std::sort(res->begin(), res->end());
      return res;
    }

    void swap(config::value& first, config::value& second)
    {
      using std::swap;
      swap(first.node_, second.node_);
      swap(first.keys_, second.keys_);
    }

    config::config()
    {
      auto path = xgetenv("VCSN_DATA_PATH", VCSN_DATADIR);
      auto flib = file_library{path, ":"};

      // Base config.
      config_tree_ = load_file(flib.find_file("config.yaml"));
      // Version file.
      merge_recurse(load_file(flib.find_file("version.yaml")),
                    config_tree_);
      // User config.
      if (!std::getenv("VCSN_NO_HOME_CONFIG"))
        {
          auto p = expand_tilda("~/.vcsn/config.yaml");
          if (boost::filesystem::exists(p))
            merge_recurse(YAML::LoadFile(p), config_tree_);
        }
    }

    config::value config::operator[](const std::string& key)
    {
      // We don't return the YAML node directly to make sure to be
      // able to change the underlying library.
      return value(config_tree_)[key];
    }
  }

  std::string configuration(const std::string& key)
  {
    // We need a unique_pointers because subscripting returns rvalues.
    auto config = std::make_unique<detail::config::value>(get_config());
    auto subkeys = std::vector<std::string>{};
    boost::split(subkeys, key, boost::is_any_of("."));

    if (subkeys.size() == 2 && subkeys[0] == "configuration")
    {
      auto env_var = "VCSN_" + boost::to_upper_copy(subkeys[1]);
      if (auto res = std::getenv(env_var.c_str()))
        return res;
    }

    for (const auto& subkey : subkeys)
      config =
        std::make_unique<detail::config::value>((*config)[subkey]);

    return config->str();
  }
}
