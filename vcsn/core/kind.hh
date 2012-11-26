#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <string>

namespace vcsn
{
  struct labels_are_empty
  {
    static std::string sname()
    {
      return "lae";
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

  template <class Kind, class GenSet>
  struct label_trait
  {
  };

  template <class GenSet>
  struct label_trait<labels_are_empty, GenSet>
  {
    using label_t = typename GenSet::empty_t;
  };

  template <class GenSet>
  struct label_trait<labels_are_letters, GenSet>
  {
    using label_t = typename GenSet::letter_t;
  };

  template <class GenSet>
  struct label_trait<labels_are_words, GenSet>
  {
    using label_t = typename GenSet::word_t;
  };

}

#endif // !VCSN_CORE_KIND_HH
