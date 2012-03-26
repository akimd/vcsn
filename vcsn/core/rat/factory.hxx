namespace vcsn
{

  /*----------.
  | factory.  |
  `----------*/

  inline
  auto
  factory::weight(std::string* w) const
    -> weight_str_container*
  {
    weight_str_container* res = new weight_str_container();
    res->push_front(w);
    return res;
  }


  inline
  auto
  factory::weight(weight_str* w, weight_str_container* l) const
    -> weight_str_container*
  {
    l->push_front(w);
    return l;
  }

  inline
  auto
  factory::weight(weight_str_container* l, weight_str* w) const
    -> weight_str_container*
  {
    // FIXME: Sounds very wrong, need a test.
    l->push_front(w);
    return l;
  }

} // namespace vcsn
