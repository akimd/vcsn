#include <vcsn/misc/random.hh>

namespace vcsn
{
  /// Generate a unique random device.
  std::mt19937& make_random_engine()
  {
    static auto res = []
      {
        if (getenv("VCSN_SEED"))
          return std::mt19937{std::mt19937::default_seed};
        else
          {
            std::random_device rd;
            return std::mt19937{rd()};
          }
      }();
    return res;
  }
}
