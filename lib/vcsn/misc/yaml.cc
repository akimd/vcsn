#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <vcsn/config.hh>
#include <vcsn/misc/file-library.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/yaml.hh>

namespace vcsn
{
  namespace detail
  {
    config::config_value::config_value(YAML::Node n)
      : node_{n}
    {}

    config::config_value::config_value(const config_value& other)
      : node_{other.node_}
    {}

    config::config_value::config_value(const config& c)
        : node_{c.config_tree}
      {}

    config::config_value&
    config::config_value::operator=(config_value rhs)
    {
      swap(*this, rhs);
      return *this;
    }

    std::string config::config_value::str() const
    {
      return as<std::string>();
    }

    config::config_value
    config::config_value::operator[](const std::string& key) const
    {
      require(node_.IsMap(),
              "YAML node is not a map. Key: ", key);
      require(node_[key].IsDefined(),
              "YAML: invalid key: ", key);
      return config_value(node_[key]);
    }

    std::vector<std::string> config::config_value::keys() const
    {
      // We generate the key only once for each config_value
      if (!keys_)
        keys_ = gen_keys();
      return *keys_;
    }

    bool config::config_value::is_valid(const std::string& key) const
    {
      return node_[key].IsDefined();
    }

    void config::config_value::remove(const std::string& key)
    {
#if VCSN_YAML_CPP_REMOVE_WORKS
      node_.remove(key);
#else
      raise("YAML: libyaml-cpp is broken, cannot remove node ", key);
#endif
    }

    using iterator = YAML::Node::iterator;

    iterator config::config_value::begin()
    {
      require(node_.IsSequence(), "YAML node is not a sequence");
      return node_.begin();
    }

    iterator config::config_value::end()
    {
      return node_.end();
    }

    void config::config_value::merge(const config_value& from)
    {
      auto dest_node = Clone(from.node_);
      merge_recurse(node_, dest_node);
      node_ = dest_node;
    }

    std::ostream& config::config_value::print(std::ostream& out) const
    {
      return out << node_;
    }

    std::unique_ptr<std::vector<std::string>>
    config::config_value::gen_keys() const
    {
      auto res = std::make_unique<std::vector<std::string>>();
      require(node_.IsMap(), "YAML node is not a map");

      for (auto e : node_)
        res->emplace_back(e.first.as<std::string>());

      // We must sort the keys to have a deterministic output
      std::sort(res->begin(), res->end());
      return res;
    }

    void swap(config::config_value& first, config::config_value& second)
    {
      using std::swap;
      swap(first.node_, second.node_);
      swap(first.keys_, second.keys_);
    }

    config::config()
    {
      const char* path = getenv("VCSN_DATA_PATH");
      if (!path)
        path = VCSN_DATADIR;
      auto flib = file_library{std::string{path}, ":"};

      auto file_path = flib.find_file("config.yaml").string();

      require(boost::filesystem::exists(file_path),
              "config file does not exists:", file_path);

      config_tree = YAML::LoadFile(file_path);
      // Now we must merge the user config
      // FIXME: Error management - altough not having a
      // HOME variable may not be an error...
      // FIXME: We should probably do a more thorought scan of the possible locations
      // Maybe something like
      //   - $XDG_CONFIG_HOME/vcsn/config
      //   - $HOME/.config/vcsn/config
      //   - $HOME/.vcsn_config.yaml
      //   - /etc/vcsn_config.yaml
      if (std::getenv("HOME") && !std::getenv("VCSN_NO_HOME_CONFIG"))
        load_home_config(std::getenv("HOME") + std::string("/VCSN_config.yaml"));
    }

    void config::load_home_config(const std::string& filename)
    {
      if (boost::filesystem::exists(filename))
      {
        auto to_merge = YAML::LoadFile(filename);
        merge_recurse(to_merge, config_tree);
      }
    }


    config::config_value config::operator[](const std::string& key)
    {
      // We don't return the YAML node directly
      // to make sure to be able to change the
      // underlying library.
      return config_value(config_tree)[key];
    }
  }

  std::string configuration(const std::string& key)
  {
    // We need a unique_pointers because subscripting returns rvalues.
    auto config = std::make_unique<detail::config::config_value>(get_config());
    auto subkeys = std::vector<std::string>{};
    boost::split(subkeys, key, boost::is_any_of("."));

    if (subkeys.size() == 2 && subkeys[0] == "configuration")
    {
      auto env_var = "VCSN_" + boost::to_upper_copy(subkeys[1]);
      if (std::getenv(env_var.c_str()))
        return std::getenv(env_var.c_str());
    }

    for (const auto& subkey : subkeys)
      config =
        std::make_unique<detail::config::config_value>((*config)[subkey]);

    return config->str();
  }

}
