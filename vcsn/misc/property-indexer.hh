#pragma once

#include <cstddef>

#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace detail
  {
    /// An index generator for the properties.
    class LIBVCSN_API property_indexer
    {
    public:
      using index_t = size_t;

      static property_indexer& instance();

      /// Give a unique id/index at each call.
      static index_t id();

      property_indexer(property_indexer const&) = delete;
      void operator=(property_indexer const&) = delete;

    private:
      property_indexer() {}

      index_t count;
    };
  } // namespace detail
} // namespace vcsn
