#pragma once

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/unordered_map.hpp>

#include <vcsn/labelset/stateset.hh>
#include <vcsn/misc/bimap.hh> // vcsn::has
#include <vcsn/misc/static-if.hh> // vcsn::has
#include <vcsn/misc/unordered_map.hh> // vcsn::has

namespace vcsn
{
  namespace detail
  {
    /// A bidirectional map from state names to state numbers.
    ///
    /// \tparam StateNameset   a valueset to manipulate the state names.
    /// \tparam Stateset       a valueset to manipulate the state indexes.
    /// \tparam Bidirectional  whether to maintain origins incrementally.
    template <typename StateNameset, typename Stateset,
              bool Bidirectional>
    class state_bimap;

    /// A bidirectional map from state names to state numbers.
    ///
    /// The lazy case: maintain the origins() map constantly.
    ///
    /// \tparam StateNameset   a valueset to manipulate the state names.
    /// \tparam Stateset       a valueset to manipulate the state indexes.
    template <typename StateNameset, typename Stateset>
    class state_bimap<StateNameset, Stateset, true>
    {
    public:
      using state_nameset_t = StateNameset;
      using state_name_t = typename state_nameset_t::value_t;

      using stateset_t = Stateset;
      using state_t = typename stateset_t::value_t;

      /// Storage for state names.
      using left_t
        = boost::bimaps::unordered_set_of<state_name_t,
                                          vcsn::hash<state_nameset_t>,
                                          vcsn::equal_to<state_nameset_t>>;
      /// Storage for state index.
      using right_t = boost::bimaps::unordered_set_of<state_t>;
      /// Bidirectional map state_name_t -> state_t;
      using bimap_t = boost::bimap<left_t, right_t>;

      using const_iterator = typename bimap_t::const_iterator;

      /// Insert a new state.
      ///
      /// Pass a "pair" state-name -> state.
      template <typename... Args>
      auto emplace(Args&&... args)
      {
        return map_.insert({ std::forward<Args>(args)... });
      }

      auto find_key(const state_name_t& s) const
      {
        return map_.left.find(s);
      }

      auto end_key() const
      {
        return map_.left.end();
      }

      /// Get the state name from a const_iterator.
      static const state_name_t& state_name(const const_iterator& i)
      {
        return i->left;
      }

      /// Get the state from a const_iterator.
      static state_t state(const const_iterator& i)
      {
        return i->right;
      }

      /// A map from state indexes to state names.
      using origins_t = typename bimap_t::right_map;
      const origins_t& origins() const
      {
        return map_.right;
      }

    private:
      bimap_t map_;
    };


    /// A bidirectional map from state names to state numbers.
    ///
    /// The strict case: compute origins() just once, at the end.
    ///
    /// \tparam StateNameset   a valueset to manipulate the state names.
    /// \tparam Stateset       a valueset to manipulate the state indexes.
    template <typename StateNameset, typename Stateset>
    class state_bimap<StateNameset, Stateset, false>
    {
    public:
      using state_nameset_t = StateNameset;
      using state_name_t = typename state_nameset_t::value_t;

      using stateset_t = Stateset;
      using state_t = typename stateset_t::value_t;

      using map_t = boost::unordered_map<state_name_t, state_t,
                                         vcsn::hash<state_nameset_t>,
                                         vcsn::equal_to<state_nameset_t>>;

      using const_iterator = typename map_t::const_iterator;

      /// Insert a new state.
      ///
      /// Pass a "pair" state-name -> state.
      template <typename... Args>
      auto emplace(Args&&... args)
      {
        return map_.emplace(std::forward<Args>(args)...);
      }

      auto find_key(const state_name_t& sn) const
      {
        return map_.find(sn);
      }

      auto end_key() const
      {
        return map_.end();
      }

      /// Get the state name from a const_iterator.
      static const state_name_t& state_name(const const_iterator& i)
      {
        return i->first;
      }

      /// Get the state from a const_iterator.
      static state_t state(const const_iterator& i)
      {
        return i->second;
      }

      /// A map from state indexes to state names.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;
      const origins_t& origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }

    private:
      map_t map_;
    };
  }
} // namespace vcsn
