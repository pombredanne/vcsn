#ifndef VCSN_ALPHABETS_SETALPHA_HH
#define VCSN_ALPHABETS_SETALPHA_HH

#include <set>

namespace vcsn
{
  template <class L>
  class set_alphabet: public L
  {
  private:
    typedef std::set<typename L::letter_t> alphabet_t;
    alphabet_t alphabet_;

  public:
    set_alphabet&
    add_letter(typename L::letter_t l)
    {
      alphabet_.insert(l);
      return *this;
    }

    typedef typename alphabet_t::const_iterator iterator_t;
    iterator_t begin() const
    {
      return alphabet_.begin();
    }

    iterator_t end() const
    {
      return alphabet_.end();
    }
  };
}

#endif // VCSN_ALPHABETS_SETALPHA_HH
