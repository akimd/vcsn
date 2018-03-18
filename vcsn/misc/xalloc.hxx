/**
 ** \file vcsn/misc/xalloc.hxx
 ** \brief Implementation for vcsn/misc/xalloc.hh.
 */

namespace vcsn
{

  /*----------------.
  | iomanipulator.  |
  `----------------*/

  inline std::ostream&
  operator<<(std::ostream& o, const iomanipulator& g)
  {
    g(o);
    return o;
  }


  /*---------------------.
  | xalloc<StoredType>.  |
  `---------------------*/

  template <typename StoredType>
  xalloc<StoredType>::xalloc()
    : index_(std::ios::xalloc())
  {}

  template <typename StoredType>
  long int
  xalloc<StoredType>::index() const
  {
    return index_;
  }

  namespace detail
  {
    // Case iword.
    template <typename StoredType>
    std::enable_if_t<std::is_integral<StoredType>{},
                     StoredType&>
    get_slot(std::ostream& ostr, int index)
    {
      return static_cast<StoredType&>(ostr.iword(index));
    }

    // Case pword.
    template <typename StoredType>
    std::enable_if_t<!std::is_integral<StoredType>{},
                     StoredType&>
    get_slot(std::ostream& ostr, int index)
    {
      return reinterpret_cast<StoredType&>
        (const_cast<const void*&>(ostr.pword(index)));
    }
  }

  template <typename StoredType>
  StoredType&
  xalloc<StoredType>::operator()(std::ostream& ostr) const
  {
    return detail::get_slot<StoredType>(ostr, index());
  }


  /*-----------.
  | set_type.  |
  `-----------*/

  template <typename StoredType>
  xalloc<StoredType>::set_type::set_type(const xalloc& slot, StoredType& data)
    : slot_(slot), data_(data)
  {}

  template <typename StoredType>
  void
  xalloc<StoredType>::set_type::operator()(std::ostream& ostr) const
  {
    slot_(ostr) = data_;
  }

  template <typename StoredType>
  typename xalloc<StoredType>::set_type
  xalloc<StoredType>::set(StoredType& data) const
  {
    return set_type(*this, data);
  }


  /*-----------.
  | get_type.  |
  `-----------*/

  template <typename StoredType>
  xalloc<StoredType>::get_type::get_type(const xalloc& slot, StoredType& data)
    : slot_(slot), data_(data)
  {
  }

  template <typename StoredType>
  void
  xalloc<StoredType>::get_type::operator()(std::ostream& ostr) const
  {
    data_ = slot_(ostr);
  }

  template <typename StoredType>
  typename xalloc<StoredType>::get_type
  xalloc<StoredType>::get(StoredType& data) const
  {
    return get_type(*this, data);
  }


  /*------------.
  | swap_type.  |
  `------------*/

  template <typename StoredType>
  xalloc<StoredType>::swap_type::swap_type(const xalloc& slot, StoredType& data)
    : slot_(slot), data_(data)
  {}

  template <typename StoredType>
  void
  xalloc<StoredType>::swap_type::operator()(std::ostream& ostr) const
  {
    std::swap(slot_(ostr), data_);
  }

  template <typename StoredType>
  typename xalloc<StoredType>::swap_type
  xalloc<StoredType>::swap(StoredType& data) const
  {
    return swap_type(*this, data);
  }

}
