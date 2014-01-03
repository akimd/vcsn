#ifndef VCSN_LABELSET_LABEL_EQUAL_TO_HH
# define VCSN_LABELSET_LABEL_EQUAL_TO_HH

// Building hashes with labels as keys is nontrivial for some labelsets.
// This class is meant to be used as a template paramterer for unordered
// associative containers, along with label_hasher.

namespace vcsn
{
  namespace ctx
  {
    template <typename LabelSet>
    class label_equal_to : public std::equal_to<typename LabelSet::value_t>
    {
    public:
      using labelset_t = LabelSet;
      using label_t = typename labelset_t::value_t;

      bool operator()(const label_t& l1, const label_t& l2) const
      {
        return LabelSet::equals(l1, l2);
      }
    };
  }
}

#endif // !VCSN_LABELSET_LABEL_EQUAL_TO_HH
