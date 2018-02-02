#pragma once

#include <optional>
#include <vector>

namespace vcsn
{
  /// Sparse Map implementation
  ///
  /// Based on Russ Cox trick allowing constant operations for sets:
  /// http://research.swtch.com/sparse
  /// Porting this concept to maps.
  template <typename Key, typename Value>
  class sparse_map
  {
    /// The Keys correspond to indexes so they have to be unsigned.
    static_assert(std::is_unsigned<Key>::value,
                  "sparse-set: requires unsigned indexes");

  public:
    sparse_map(size_t max_size = 0)
      : dense_{}
      , sparse_{}
    {
      set_max_size(max_size);
    }

    void set_max_size(size_t max_size)
    {
      sparse_.resize(max_size);
    }

    template <typename K, typename V>
    bool emplace(K&& k, V&& v)
    {
      return insert(std::make_pair(k, v));
    }

    /// Insert new value.
    ///
    /// \returns true if the element was succesfully inserted, false if it was
    //           already present in the set.
    bool insert(const std::pair<Key, Value>& p)
    {
      if (has(p.first))
        return false;
      else
        {
          if (sparse_.size() <= p.first)
            sparse_.resize(p.first + 1);
          if (curr_ < dense_.size())
            dense_[curr_] = p;
          else
            dense_.push_back(p);
          sparse_[p.first] = curr_;
          ++curr_;
          return true;
        }
    }

    bool has(Key k) const
    {
      return (k < sparse_.capacity()
              && sparse_[k] < curr_
              && dense_[sparse_[k]].first == k);
    }

    /// Access an element.
    ///
    /// Creates a new value if the key is not present
    Value& operator[](Key k)
    {
      if (!has(k))
        emplace(k, Value());
      return dense_[sparse_[k]].second;
    }

    /// Erase an element.
    ///
    /// Does nothing if the element is not present.
    void erase(Key k)
    {
      if (has(k))
        {
          Key last = dense_[curr_ - 1].first;
          dense_[sparse_[k]].first = last;
          sparse_[last] = sparse_[k];
          curr_--;
        }
    }

    /// Flush all elements from the set.
    void clear()
    {
      curr_ = 0;
    }

    auto begin()
    {
      return dense_.begin();
    }

    auto end()
    {
      return dense_.begin() + curr_;
    }

    auto begin() const
    {
      return dense_.begin();
    }

    auto end() const
    {
      return dense_.begin() + curr_;
    }

  private:

    std::vector<std::pair<Key, Value>> dense_;
    std::vector<Key> sparse_;
    Key curr_ = 0;
  };

  template <typename Key, typename Value>
  bool
  has(const sparse_map<Key, Value>& s, Key k)
  {
    return s.has(k);
  }
}
