#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <string>

namespace vcsn
{
  struct labels_are_unit
  {
    static std::string sname()
    {
      return "lau";
    }
  };

  struct labels_are_letters
  {
    static std::string sname()
    {
      return "lal";
    }
  };

  struct labels_are_words
  {
    static std::string sname()
    {
      return "law";
    }
  };

  template <class Kind, class LabelSet>
  struct label_trait
  {
  };

  template <class LabelSet>
  struct label_trait<labels_are_unit, LabelSet>
  {
    using label_t = typename LabelSet::empty_t;
  };

  template <class LabelSet>
  struct label_trait<labels_are_letters, LabelSet>
  {
    using label_t = typename LabelSet::letter_t;
  };

  template <class LabelSet>
  struct label_trait<labels_are_words, LabelSet>
  {
    using label_t = typename LabelSet::word_t;
  };

}

#endif // !VCSN_CORE_KIND_HH
