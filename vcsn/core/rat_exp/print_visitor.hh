#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_

# include <iostream>
# include <core/rat_exp/visitor.hh>

namespace vcsn {
  namespace rat_exp {

    template<class WeightSet>
    class PrintVisitor : public visitor_traits<WeightSet>::ConstVisitor
    {
    public:
      PrintVisitor(std::ostream &out);
      virtual ~PrintVisitor();
    public:
      virtual void visit(const RatExpNode<WeightSet> &          v);
      virtual void visit(const RatExpConcat<WeightSet> &        v);
      virtual void visit(const RatExpPlus<WeightSet> &          v);
      virtual void visit(const RatExpKleene<WeightSet> &        v);
      virtual void visit(const RatExpOne<WeightSet> &           v);
      virtual void visit(const RatExpZero<WeightSet> &          v);
      virtual void visit(const RatExpWord<WeightSet> &          v);
    private:
      std::ostream &out_;
    };

  } // !rat_exp
} // !vcsn

# include <core/rat_exp/print_visitor.hxx>

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HH_
