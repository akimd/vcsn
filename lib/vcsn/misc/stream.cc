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

  void eat(std::istream& is, char c)
  {
    require (is.peek() == c,
             "unexpected: ", str_escape(is.peek()),
             ": expected ", str_escape(c));
    is.ignore();
  }

  void eat(std::istream& is, const std::string& expect)
  {
    std::string s;
    char c;
    size_t cnt = expect.size();
    while (cnt && is >> c)
      {
        s.append(1, c);
        --cnt;
      }
    require(s == expect,
            "unexpected: ", str_escape(s), ": expected ", str_escape(expect));
  }

  // http://stackoverflow.com/questions/2602013.
  std::string
  get_file_contents(const std::string& file)
  {
    std::ifstream in(file.c_str(), std::ios::in | std::ios::binary);
    require(in.good(), "cannot read file: ", file, ": ", strerror(errno));

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
        require(res->good(),
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
        require(res->good(),
                "cannot open ", file, " for writing: ", strerror(errno));
      }
    return res;
  }

  void fail_reading(std::istream& is, std::string explanation)
  {
    is.clear();
    std::string buf;
    is >> buf;
    if (!is.good())
      // This shouldn't really happen; however it's best to fail cleanly.
      is.clear();
    if (!buf.empty())
      explanation += ": " + str_escape(buf);
    raise(explanation);
  }
}
