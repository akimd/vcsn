#include <vcsn/misc/stream.hh>

#include <cassert>
#include <cstring> // strerror
#include <istream>
#include <fstream>

#include <vcsn/misc/raise.hh>

namespace vcsn
{

  std::ostream cnull{nullptr};
  std::wostream wcnull{nullptr};

  std::string
  bracketed(std::istream& i, const char lbracket, const char rbracket)
  {
    assert(i.peek() == lbracket);
    i.ignore();
    size_t level = 1;
    std::ostringstream o;
    int c;
    while ((c = i.get()) != -1)
      {
        if (c == lbracket)
          ++level;
        else if (c == rbracket
                 && !--level)
          return o.str();
        o << char(c);
      }
    raise("missing  ", str_escape(rbracket), " after ",
          str_escape(lbracket), o.str());
  }


  bool
  equal_files(const std::string& fn1, const std::string& fn2)
  {
    std::ifstream f1(fn1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream f2(fn2, std::ifstream::binary|std::ifstream::ate);

    if (f1.fail() || f2.fail())
      return false;

    if (f1.tellg() != f2.tellg())
      return false;

    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
  }


  char get_char(std::istream& i)
  {
    int res = i.get();
    if (res == '\\')
      switch (int c = i.get())
        {
          // Usual escapes.
#define CASE(Key, Value)                                \
          case Key: res = Value; i.ignore(); break
          CASE('a', '\a');
          CASE('b', '\b');
          CASE('f', '\f');
          CASE('n', '\n');
          CASE('r', '\r');
          CASE('t', '\t');
          CASE('v', '\v');
#undef CASE

          // Hexadecimal escapes.
        case 'x':
          {
            // Handle hexadecimal escape.
            int c1 = i.get();
            require(c1 != EOF,
                    "get_char: unexpected end-of-file"
                    " after: \\x");
            require(isxdigit(c1),
                    "get_char: invalid escape: \\x", char(c1));
            int c2 = i.get();
            require(c2 != EOF,
                    "get_char: unexpected end-of-file"
                    " after: \\x", char(c1));
            require(isxdigit(c2),
                    "get_char: invalid escape: \\x",
                    char(c1), char(c2));
            res = std::stoi(std::string{char(c1), char(c2)}, nullptr, 16);
          }
          break;

          // Other escapes, e.g., \\, \", \', etc.  \(, \) and \-
          // are used in setalpha::make, e.g., char_letters(\(\-\)).
        default:
          require(!std::isalnum(c),
                  "get_char: invalid escape: \\", char(c), " in \\",
                  char(c), i);
          res = c;
          break;
        }
    require(res != EOF,
            "get_char: unexpected end-of-file");
    return res;
  }

  char eat(std::istream& is, char c)
  {
    if (is.peek() == c)
      {
        is.ignore();
        return c;
      }
    else
      fail_reading(is, "expected ", str_escape(c), ", got");
  }

  const std::string& eat(std::istream& is, const std::string& expect)
  {
    std::string s;
    char c;
    size_t cnt = expect.size();
    while (cnt && is >> c)
      {
        s.append(1, c);
        --cnt;
      }
    VCSN_REQUIRE(s == expect,
                 "unexpected: ", str_escape(s),
                 ": expected ", str_escape(expect));
    return expect;
  }

  // http://stackoverflow.com/questions/2602013.
  std::string
  get_file_contents(const std::string& file)
  {
    std::ifstream in(file.c_str(), std::ios::in | std::ios::binary);
    VCSN_REQUIRE(in.good(), "cannot read file: ", file, ": ", strerror(errno));

    std::string res;
    in.seekg(0, std::ios::end);
    res.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&res[0], res.size());
    in.close();
    return res;
  }

  std::shared_ptr<std::istream>
  open_input_file(const std::string& file)
  {
    std::shared_ptr<std::istream> res;
    if (file.empty() || file == "-")
      res.reset(&std::cin, [](...){});
    else
      {
        res.reset(new std::ifstream(file.c_str()));
        VCSN_REQUIRE(res->good(),
                     "cannot open ", file, " for reading: ", strerror(errno));
      }
    return res;
  }

  std::shared_ptr<std::ostream>
  open_output_file(const std::string& file)
  {
    std::shared_ptr<std::ostream> res;
    if (file.empty() || file == "-")
      res.reset(&std::cout, [](...){});
    else
      {
        res.reset(new std::ofstream(file.c_str()));
        VCSN_REQUIRE(res->good(),
                     "cannot open ", file, " for writing: ", strerror(errno));
      }
    return res;
  }

  void skip_space(std::istream& is)
  {
    while (isspace(is.peek()))
      is.ignore();
  }
}
