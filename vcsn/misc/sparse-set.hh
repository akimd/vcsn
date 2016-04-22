#pragma once

#include <vector>

namespace vcsn
{
  class sparse_set
  {
  public:
    sparse_set(unsigned max_size = 0)
      : dense_{}
      , sparse_{}
    {
      set_max_size(max_size);
    }

    void set_max_size(unsigned max_size)
    {
      sparse_.resize(max_size);
    }

    bool emplace(unsigned e)
    {
      return insert(e);
    }

    bool insert(unsigned e)
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

    bool has(unsigned e) const
    {
      return e < sparse_.capacity()
             && sparse_[e] < curr_
             && dense_[sparse_[e]] == e;
    }

    void erase(unsigned e)
    {
      if (has(e))
        {
          unsigned last = dense_[curr_ - 1];
          dense_[sparse_[e]] = last;
          sparse_[last] = sparse_[e];
          --curr_;
        }
    }

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
    std::vector<unsigned> dense_;
    std::vector<unsigned> sparse_;
    unsigned curr_ = 0;
  };

  inline
  bool
  has(const sparse_set& s, unsigned e)
  {
    return s.has(e);
  }
}
