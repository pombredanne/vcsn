#ifndef VCSN_ALPHABETS_CHAR_HH
# define VCSN_ALPHABETS_CHAR_HH

# include <string>
# include <iostream>

# include <vcsn/empty.hh>

namespace vcsn
{
  class char_letters
  {
  public:
    using empty_t = ::vcsn::empty_t;
    using letter_t = char;
    using word_t = std::string;

    word_t
    to_word(const empty_t) const
    {
      return {};
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
    concat(const word_t l, const letter_t r) const
    {
      return l + r;
    }

    word_t
    concat(const letter_t l, const word_t r) const
    {
      return l + r;
    }

    word_t
    concat(const word_t l, const word_t r) const
    {
      return l + r;
    }

    word_t
    identity() const
    {
      return {};
    }

    bool
    is_identity(const word_t& w) const
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

    empty_t
    transpose(empty_t) const
    {
      return {};
    }

    bool
    equals(empty_t, empty_t) const
    {
      return true;
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
    is_letter(const empty_t&) const
    {
      return false;
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

  protected:
    /// Use special().
    letter_t special_letter() const { return 255; }

  public:
    std::ostream&
    print(std::ostream& o, const empty_t&) const
    {
      return o << "EMPTY";
    }

    std::ostream&
    print(std::ostream& o, const letter_t& l) const
    {
      if (l != special_letter())
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
      if (l != special_letter())
        return {l};
      else
        return {};
    }

    std::string
    format(const word_t& w) const
    {
      size_t s = w.size();

      if (s == 0)
        return "\\e";

      // If the string starts or ends with the special letter, skip
      // it.  If the resulting string is empty, format it this way.
      // (We DON'T want to format it as "\\e".)
      if (w[0] == special_letter())
        return (s == 1) ? "" : w.substr(1);

      if (s > 1 && w[s - 1] == special_letter())
        return w.substr(0, s - 1);

      return w;
    }

    // Special character, used to label transitions
    // from pre() and post()
    template<class T = letter_t>
    T special() const;

    // word_t mirror(const word_t& w)
  };

  template<>
  inline
  char_letters::empty_t
  char_letters::special<char_letters::empty_t>() const
  {
    return {};
  }

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
