#pragma once

#include <vector>

namespace vcsn
{
  /// Sparse Set implementation
  ///
  /// Based on Russ Cox trick allowing constant operations:
  /// http://research.swtch.com/sparse
  template <typename T>
  class sparse_set
  {
    /// The template parameter corresponds to indexes so it has to  be unsigned.
    static_assert(std::is_unsigned<T>::value,
                  "sparse-set: requires unsigned indexes");

  public:
    sparse_set(size_t max_size = 0)
      : dense_{}
      , sparse_{}
    {
      set_max_size(max_size);
    }

    /// Construct with a container.
    ///
    /// Use size for SFINAE as it is not featured in sparse_set.
    /// Hence, this is not a copy-constructor.
    template <template <typename Elt> class Container,
              typename = decltype(std::declval<Container<T>>().size())>
    sparse_set(const Container<T>& cont)
      : dense_{}
      , sparse_{}
    {
      set_max_size(cont.size());
      for (const auto& elt : cont)
        insert(elt);
    }

    /// Set current vector size.
    void set_max_size(size_t max_size)
    {
      sparse_.resize(max_size);
    }

    bool emplace(T e)
    {
      return insert(e);
    }

    /// Insert new value.
    ///
    /// \returns true if the element was succesfully inserted, false if it was
    //           already present in the set.
    bool insert(T e)
    {
      if (has(e))
        return false;
      if (sparse_.size() <= e)
        sparse_.resize(e + 1);
      if (curr_ < dense_.size())
        dense_[curr_] = e;
      else
        dense_.push_back(e);
      sparse_[e] = curr_;
      ++curr_;
      return true;
    }

    bool has(T e) const
    {
      return e < sparse_.capacity()
             && sparse_[e] < curr_
             && dense_[sparse_[e]] == e;
    }

    /// Erase an element.
    ///
    /// Does nothing if the element is not present.
    void erase(T e)
    {
      if (has(e))
        {
          T last = dense_[curr_ - 1];
          dense_[sparse_[e]] = last;
          sparse_[last] = sparse_[e];
          --curr_;
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
    /// Actual elements of the set, not sorted.
    std::vector<T> dense_;
    /// Indexes of elements in the dense_ vector.
    std::vector<T> sparse_;
    /// Current number of elements.
    T curr_ = 0;
  };

  template <typename T>
  inline
  bool
  has(const sparse_set<T>& s, T e)
  {
    return s.has(e);
  }
}
