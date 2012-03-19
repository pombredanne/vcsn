#ifndef VCSN_RAT_NODE_HH_
# define VCSN_RAT_NODE_HH_

# include <list>
# include <string>

# include <core/rat/node.fwd.hh>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    class RatExp
    {
    public:
      virtual ~RatExp() = 0;
    };

    template <class WeightSet>
    class node : public RatExp
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      enum DynamicType
        {
          CONCAT,
          PLUS,
          KLEENE,
          ONE,
          ZERO,
          WORD,
        };
      enum WeightType
        {
          L_WEIGHT,
          LR_WEIGHT
        };
    protected:
      typedef typename visitor_traits<WeightSet>::Visitor Visitor;
      typedef typename visitor_traits<WeightSet>::ConstVisitor ConstVisitor;

    public:
      node();
      virtual ~node() = 0;
    public:
      virtual void accept(Visitor &v) = 0;
      virtual void accept(ConstVisitor &v) const = 0;
    public:
      virtual DynamicType type() const = 0;
      virtual WeightType weight_type() const = 0;
    };

    template <class WeightSet>
    class LRWeightNode : public node<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef node<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      LRWeightNode();
      LRWeightNode(const weightset_t& ws);
    public:
      virtual ~LRWeightNode() = 0;
    public:
      const weightset_t &get_weight_set() const;
      const weight_t &left_weight() const;
      weight_t &left_weight();

      const weight_t &right_weight() const;
      weight_t &right_weight();

      virtual WeightType weight_type() const;
    protected:
      const static weightset_t st_ws_;
      const weightset_t& ws_;
      weight_t lw_;
      weight_t rw_;
    };

    template<class WeightSet>
    const typename LRWeightNode<WeightSet>::weightset_t
    LRWeightNode<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class LWeightNode : public node<WeightSet>
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef node<WeightSet> super_type;
      typedef typename super_type::WeightType WeightType;
    protected:
      LWeightNode();
      LWeightNode(const weightset_t& ws);
    public:
      virtual ~LWeightNode() = 0;
    public:
      const weightset_t &get_weight_set() const;
      const weight_t &left_weight() const;
      weight_t &left_weight();

      virtual WeightType weight_type() const;
    protected:
      const static weightset_t st_ws_;
      const weightset_t& ws_;
      weight_t lw_;
    };

    template <class WeightSet>
    const typename LWeightNode<WeightSet>::weightset_t
    LWeightNode<WeightSet>::st_ws_ = WeightSet();

    template <class WeightSet>
    class concat : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      concat();
      concat(weightset_t& ws);
      virtual ~concat();
    public:
      virtual DynamicType type() const { return node_t::CONCAT; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      concat<WeightSet>& push_back(node_t* elt);
      concat<WeightSet>& push_front(node_t* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename node_t::Visitor& v);
      virtual void accept(typename node_t::ConstVisitor& v) const;

    private:
      nodes_t sub_node_;
    };

    template <class WeightSet>
    class plus : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef std::list<node_t*> nodes_t;

      typedef typename nodes_t::const_iterator         const_iterator;
      typedef typename nodes_t::iterator               iterator;
      typedef typename nodes_t::const_reverse_iterator const_reverse_iterator;
      typedef typename nodes_t::reverse_iterator       reverse_iterator;
    public:
      plus();
      plus(const weightset_t& ws);
      virtual ~plus();
    public:
      virtual DynamicType type() const { return node_t::PLUS; };

      const_iterator begin() const;
      const_iterator end() const;
      iterator begin();
      iterator end();
      const_reverse_iterator rbegin() const;
      const_reverse_iterator rend() const;
      reverse_iterator rbegin();
      reverse_iterator rend();
    public:
      plus<WeightSet>& push_back(node_t* elt);
      plus<WeightSet>& push_front(node_t* elt);
      size_t size() const;
      void erase(iterator it);

    public:
      virtual void accept(typename node_t::Visitor& v);
      virtual void accept(typename node_t::ConstVisitor& v) const;

    private:
      nodes_t sub_node_;
    };

    template <class WeightSet>
    class kleene : public LRWeightNode<WeightSet>
    {
    public:
      typedef LRWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;

      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      kleene(node_t* sub_exp);
      kleene(node_t* sub_exp, const weightset_t& ws);
      virtual ~kleene();
    public:
      node_t *get_sub();
      const node_t *get_sub() const;
    public:
      virtual DynamicType type() const { return node_t::KLEENE; };

      virtual void accept(typename node_t::Visitor &v);
      virtual void accept(typename node_t::ConstVisitor &v) const;

    private:
      node_t *sub_exp_;
    };

    template <class WeightSet>
    class one : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      one();
      virtual ~one();
    public:
      virtual DynamicType type() const { return node_t::ONE; };

      virtual void accept(typename node_t::Visitor &v);
      virtual void accept(typename node_t::ConstVisitor &v) const;
    };

    template <class WeightSet>
    class zero : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      zero();
      virtual ~zero();
    public:
      virtual DynamicType type() const { return node_t::ZERO; };

      virtual void accept(typename node_t::Visitor &v);
      virtual void accept(typename node_t::ConstVisitor &v) const;
    };

    template <class WeightSet>
    class word : public LWeightNode<WeightSet>
    {
    public:
      typedef LWeightNode<WeightSet> super_type;
      typedef node<WeightSet> node_t;
      typedef typename node_t::DynamicType DynamicType;
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      word(std::string* word);
      word(std::string* word, const weightset_t& ws);
      virtual ~word();
    public:
      virtual DynamicType type() const { return node_t::WORD; };

      virtual void accept(typename node_t::Visitor &v);
      virtual void accept(typename node_t::ConstVisitor &v) const;
      std::string *get_word();
      const std::string *get_word() const;
    private:
      std::string *word_;
    };

  } // rat
} // vcsn

#include <core/rat/node.hxx>

#endif // !VCSN_RAT_NODE_HH_
