#ifndef VCSN_LABELSET_LABEL_HASHER_HH
# define VCSN_LABELSET_LABEL_HASHER_HH

// Building hashes with labels as keys is nontrivial for some labelsets.
// This class is meant to be used as a template paramterer for unordered
// associative containers, along with label_equal_to.

namespace vcsn
{
  namespace ctx
  {
    template <typename LabelSet>
    class label_hasher : public std::hash<typename LabelSet::value_t>
    {
    public:
      using labelset_t = LabelSet;
      using label_t = typename labelset_t::value_t;

      size_t operator()(const label_t& l) const
      {
        return labelset_t::hash(l);
      }
    };
  }
}

#endif // !VCSN_LABELSET_LABEL_HASHER_HH
