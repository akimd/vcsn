#pragma once

// Modified version of x3/ast/error_reporting.hpp which does not
// provide enough customization points (e.g., format of file:line).

#include <ostream>

#include <boost/filesystem/path.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace vcsn::dyn::parser
{
  // Tag used to get our error handler from the context.
  struct error_handler_tag;

  namespace x3 = boost::spirit::x3;

  template <typename Iterator>
  class error_handler
  {
  public:

    using iterator_type = Iterator;

    error_handler(Iterator first, Iterator last, std::ostream& err_out,
                  std::string file = "", int tabs = 4)
      : err_out(err_out)
      , file(file)
      , tabs(tabs)
      , pos_cache(first, last) {}

    using result_type = void;

    void operator()(Iterator err_pos, std::string const& error_message) const;
    void operator()(Iterator err_first, Iterator err_last, std::string const& error_message) const;
    void operator()(x3::position_tagged pos, std::string const& message) const
    {
      auto where = pos_cache.position_of(pos);
      (*this)(where.begin(), where.end(), message);
    }

    template <typename AST>
    void tag(AST& ast, Iterator first, Iterator last)
    {
      return pos_cache.annotate(ast, first, last);
    }

    boost::iterator_range<Iterator> position_of(x3::position_tagged pos) const
    {
      return pos_cache.position_of(pos);
    }

  private:

    void print_file_line(std::size_t line) const;
    // Print source line.
    void print_line(Iterator line_start, Iterator last) const;
    void print_indicator(Iterator& line_start, Iterator last, char ind) const;
    void skip_whitespace(Iterator& err_pos, Iterator last) const;
    void skip_non_whitespace(Iterator& err_pos, Iterator last) const;
    Iterator get_line_start(Iterator first, Iterator pos) const;
    std::size_t position(Iterator i) const;

    std::ostream& err_out;
    std::string file;
    int tabs;
    x3::position_cache<std::vector<Iterator>> pos_cache;
  };

  template <typename Iterator>
  void error_handler<Iterator>::print_file_line(std::size_t line) const
  {
    if (!file.empty())
      {
        namespace fs = boost::filesystem;
        err_out << fs::path(file).generic_string() << ": ";
      }
    err_out << line << ": ";
  }

  template <typename Iterator>
  void error_handler<Iterator>::print_line(Iterator start, Iterator last) const
  {
    auto end = start;
    while (end != last)
      {
        auto c = *end;
        if (c == '\r' || c == '\n')
          break;
        else
          ++end;
      }
    using char_type = typename std::iterator_traits<Iterator>::value_type;
    auto line = std::basic_string<char_type>{start, end};
    err_out << boost::locale::conv::utf_to_utf<char>(line) << std::endl;
  }

  template <typename Iterator>
  void error_handler<Iterator>::print_indicator(Iterator& start, Iterator last, char ind) const
  {
    for (; start != last; ++start)
      {
        auto c = *start;
        if (c == '\r' || c == '\n')
          break;
        else if (c == '\t')
          for (int i = 0; i < tabs; ++i)
            err_out << ind;
        else
          err_out << ind;
      }
  }

  template <typename Iterator>
  void error_handler<Iterator>::skip_whitespace(Iterator& err_pos, Iterator last) const
  {
    // make sure err_pos does not point to white space
    while (err_pos != last)
      {
        char c = *err_pos;
        if (std::isspace(c))
          ++err_pos;
        else
          break;
      }
  }

  template <typename Iterator>
  void error_handler<Iterator>::skip_non_whitespace(Iterator& err_pos, Iterator last) const
  {
    // make sure err_pos does not point to white space
    while (err_pos != last)
      {
        char c = *err_pos;
        if (std::isspace(c))
          break;
        else
          ++err_pos;
      }
  }

  template <class Iterator>
  inline Iterator error_handler<Iterator>::get_line_start(Iterator first, Iterator pos) const
  {
    Iterator latest = first;
    for (Iterator i = first; i != pos; ++i)
      if (*i == '\r' || *i == '\n')
        latest = i;
    return latest;
  }

  template <typename Iterator>
  std::size_t
  error_handler<Iterator>::position(Iterator i) const
  {
    std::size_t line { 1 };
    typename std::iterator_traits<Iterator>::value_type prev { 0 };

    for (Iterator pos = pos_cache.first(); pos != i; ++pos) {
      auto c = *pos;
      switch (c) {
      case '\n':
        if (prev != '\r') ++line;
        break;
      case '\r':
        if (prev != '\n') ++line;
        break;
      default:
        break;
      }
      prev = c;
    }

    return line;
  }

  template <typename Iterator>
  void
  error_handler<Iterator>::operator()(Iterator err_pos,
                                      std::string const& error_message) const
  {
    auto first = pos_cache.first();
    auto last = pos_cache.last();

    // make sure err_pos does not point to white space
    skip_whitespace(err_pos, last);

    //print_file_line(position(err_pos));
    err_out << error_message << std::endl;

    Iterator start = get_line_start(first, err_pos);
    if (start != first)
      ++start;
    print_line(start, last);
    print_indicator(start, err_pos, ' ');
    err_out << "^_";
  }

  template <typename Iterator>
  void error_handler<Iterator>::operator()(Iterator err_first, Iterator err_last,
                                           std::string const& error_message) const
  {
    auto first = pos_cache.first();
    auto last = pos_cache.last();

    // make sure err_pos does not point to white space
    skip_whitespace(err_first, last);

    //print_file_line(position(err_first));
    err_out << error_message << std::endl;

    Iterator start = get_line_start(first, err_first);
    if (start != first)
      ++start;
    print_line(start, last);
    print_indicator(start, err_first, ' ');
    print_indicator(start, err_last, '~');
    err_out << " <<-- Here";
  }
}
