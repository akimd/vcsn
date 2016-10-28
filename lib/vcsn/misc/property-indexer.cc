#include <vcsn/misc/property-indexer.hh>

namespace vcsn
{
  namespace detail
  {
    property_indexer& property_indexer::instance()
    {
      static property_indexer instance;
      return instance;
    }

    property_indexer::index_t property_indexer::id()
    {
      return instance().count++;
    }
  } // namespace detail
} // namespace vcsn
