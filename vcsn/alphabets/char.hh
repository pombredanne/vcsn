#ifndef VCSN_ALPHABETS_CHAR_HH
# define VCSN_ALPHABETS_CHAR_HH

# include <string>
# include <iostream>

# include <vcsn/misc/escape.hh>

namespace vcsn
{
  class char_letters
  {
  public:
    using letter_t = char;
    using word_t = std::string;

    static std::string sname()
    {
      return "char_letter";
    }

    virtual std::string vname(bool = true) const
    {
      return sname();
    }

    word_t
    to_word(const letter_t l) const
    {
      return {l};
    }

    const word_t&
    to_word(const word_t& l) const
    {
      return l;
    }

    word_t
    concat(const letter_t l, const letter_t r) const
    {
      return {l, r};
    }

    word_t
    concat(const word_t& l, const letter_t r) const
    {
      return l + r;
    }

    word_t
    concat(const letter_t l, const word_t& r) const
    {
      return l + r;
    }

    word_t
    concat(const word_t& l, const word_t& r) const
    {
      return l + r;
    }

    word_t
    empty_word() const
    {
      return {};
    }

    bool
    is_empty_word(const word_t& w) const
    {
      return w.empty();
    }

    word_t
    transpose(const word_t& w) const
    {
      // C++11 lacks std::rbegin/rend...
      return {w.rbegin(), w.rend()};
    }

    letter_t
    transpose(letter_t l) const
    {
      return l;
    }

    bool
    equals(const letter_t& l1, const letter_t& l2) const
    {
      return l1 == l2;
    }

    bool
    equals(const word_t& w1, const word_t& w2) const
    {
      return w1 == w2;
    }

    bool
    is_letter(const letter_t&) const
    {
      return true;
    }

    bool
    is_letter(const word_t& w) const
    {
      return w.size() == 1;
    }

    static constexpr letter_t identity_letter() { return 0; }

  private:
    /// Use special().
    static constexpr letter_t special_letter() { return 255; }

  public:
    std::ostream&
    print(std::ostream& o, const letter_t& l) const
    {
      if (l != identity_letter() && l != special_letter())
        o << l;
      return o;
    }

    std::ostream&
    print(std::ostream& o, const word_t& w) const
    {
      return o << format(w);
    }

    std::string
    format(const letter_t l) const
    {
      if (l == identity_letter() || l == special_letter())
        return {};
      else
        return str_escape(l);
    }

    std::string
    format(const word_t& w) const
    {
      size_t s = w.size();

      std::string res;
      if (s == 0
          || (s == 1 && w[0] == identity_letter()))
        res = "\\e";

      // If the string starts or ends with the special letter, skip
      // it.  If the resulting string is empty, format it this way.
      // (We DON'T want to format it as "\\e".)
      else if (w[0] == special_letter())
        res = (s == 1) ? "" : str_escape(w.substr(1));
      else if (s > 1 && w[s - 1] == special_letter())
        res = str_escape(w.substr(0, s - 1));
      else
        res = str_escape(w);
      return res;
    }

    // Special character, used to label transitions
    // from pre() and post()
    template<class T = letter_t>
    T special() const;
  };

  template<>
  inline
  char_letters::letter_t
  char_letters::special<char_letters::letter_t>() const
  {
    return special_letter();
  }

  template<>
  inline
  char_letters::word_t
  char_letters::special<char_letters::word_t>() const
  {
    return {special_letter()};
  }

}

#endif // !VCSN_ALPHABETS_CHAR_HH
