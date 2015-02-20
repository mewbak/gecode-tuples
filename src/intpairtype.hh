/*
 *  Authors:
 *    Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *    Christian Schulte, 2008-2014
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software, to deal in the software without restriction,
 *  including without limitation the rights to use, copy, modify, merge,
 *  publish, distribute, sublicense, and/or sell copies of the software,
 *  and to permit persons to whom the software is furnished to do so, subject
 *  to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* Authors:
   Patrik Broman

   Copyright:
   Patrik Broman
*/

#ifndef __MPG_INTPAIR_HH__
#define __MPG_INTPAIR_HH__
#include <iostream>
#include <sstream>
#include <climits>
#include <vector>

#include <gecode/kernel.hh>

using Gecode::Advisor;
using Gecode::ConstView;
using Gecode::Delta;
using Gecode::DerivedView;
using Gecode::Exception;
using Gecode::ModEvent;
using Gecode::ModEventDelta;
using Gecode::PropCond;
using Gecode::Propagator;
using Gecode::Space;
using Gecode::VarArgArray;
using Gecode::VarArray;
using Gecode::VarImpVar;
using Gecode::VarImpView;
using Gecode::AFC;
using Gecode::Activity;
using Gecode::Home;
using Gecode::ViewArray;
using Gecode::Rnd;
using Gecode::VarBranch;
using Gecode::ValBranch;
using Gecode::BranchTbl;
using Gecode::VoidFunction;
using Gecode::MeritBase;
using Gecode::ViewSel;
using Gecode::ViewSelMaxTbl;
using Gecode::ViewSelMinTbl;
using Gecode::ViewSelMax;
using Gecode::ViewSelMin;
using Gecode::ViewSelNone;
using Gecode::ViewSelRnd;
using Gecode::MeritFunction;
using Gecode::MeritDegree;
using Gecode::MeritActivity;
using Gecode::ValSel;
using Gecode::ValCommit;
using Gecode::NGL;
using Gecode::ExecStatus;
using Gecode::ES_FAILED;
using Gecode::ES_OK;
using Gecode::me_failed;
using Gecode::ViewValNGL;
using Gecode::ValSelCommit;
using Gecode::ValSelCommitBase;
using Gecode::ValSelFunction;
using Gecode::ValCommitFunction;
using Gecode::ViewValBrancher;
using Gecode::TieBreak;
using Gecode::function_cast;
using Gecode::BrancherHandle;

//using namespace MPG::IntPair;
//using namespace MPG;
/*
// exceptions
namespace MPG { namespace IntPair {
    
  class OutOfLimits : public Exception {
  public:
    OutOfLimits(const char* l)
      : Exception(l,"Number out of limits") {}
  };
  class VariableEmptyDomain : public Exception {
  public:
    VariableEmptyDomain(const char* l)
      : Exception(l,"Attempt to create variable with empty domain") {}
  };
  class UnknownBranching : public Exception {
  public:
    UnknownBranching(const char* l)
      : Exception(l,"Unknown branching (variable or value)") {}
  };
}}
*/ 

// variable implementation

namespace MPG { namespace IntPair {

    /*
  // limits
  namespace Limit {
    const int max = (INT_MAX / 2) - 1;
    const int min = -max;
  }
  // delta for advisors
  class IntDelta : public Delta {
  private:
    int l, u;
  public:
    IntDelta(int min, int max) : l(min), u(max) {}
    int min(void) const {
      return l;
    }
    int max(void) const {
      return u;
    }
    }; */

      struct Pair {
	int x, y;
	Pair(int x, int y) : x(x), y(y) {};
      };

    template<class Char, class Traits>
    std::basic_ostream<Char,Traits>&
    operator <<(std::basic_ostream<Char,Traits>& os, const Pair& p) {
      std::basic_ostringstream<Char,Traits> s;
      s.copyfmt(os); s.width(0);
      s << '<' << p.x << ',' << p.y << '>';
      return os << s.str();
      };


    class IntPairVarImp : public IntPairVarImpBase {
    protected:

    std::vector<Pair> domain;

  public:
    IntPairVarImp(Space& home, int x_min, int y_min, int x_max, int y_max)
      : IntPairVarImpBase(home) {
      for(int x=x_min; x<=x_max; x++)
	for(int y=y_min; y<=y_max; y++)
	  domain.push_back(Pair(x,y));
    }
    // access operations
      Pair min(void) const {
	return Pair(domain[0]);
      }

      Pair max(void) const {
	return Pair(domain.back());
      }
    // assignment test
    bool assigned(void) const {
      return domain.size()==1;
    }
    /*
    // modification operations
    ModEvent lq(Space& home, int n) {
      if (n >= u) return ME_INT_NONE;
      if (n < l) return ME_INT_FAILED;
      IntDelta d(n+1,u); u = n;
      return notify(home, assigned() ? ME_INT_VAL : ME_INT_MAX, d);
    }
    ModEvent gq(Space& home, int n) {
      if (n <= l) return ME_INT_NONE;
      if (n > u) return ME_INT_FAILED;
      IntDelta d(l,n-1); l = n;
      return notify(home, assigned() ? ME_INT_VAL : ME_INT_MIN, d);
    }
    */
    // subscriptions
    /*
    void subscribe(Space& home, Propagator& p, PropCond pc, 
                   bool schedule=true) {
      IntVarImpBase::subscribe(home,p,pc,assigned(),schedule);
    }
    void subscribe(Space& home, Advisor& a) {
      IntVarImpBase::subscribe(home,a,assigned());
    }
    void cancel(Space& home, Propagator& p, PropCond pc) {
      IntVarImpBase::cancel(home,p,pc,assigned());
    }
    void cancel(Space& home, Advisor& a) {
      IntVarImpBase::cancel(home,a,assigned());
      } */
    // copying
    IntPairVarImp(Space& home, bool share, IntPairVarImp& y)
      : IntPairVarImpBase(home,share,y), domain(y.domain) {}
    IntPairVarImp* copy(Space& home, bool share) {
      if (copied()) 
        return static_cast<IntPairVarImp*>(forward());
      else
        return new (home) IntPairVarImp(home,share,*this);
    }
    // delta information
    /*
    static int min(const Delta& d) {
      return static_cast<const IntDelta&>(d).min();
    }
    static int max(const Delta& d) {
      return static_cast<const IntDelta&>(d).max();
      }*/
  };

}}


// variable
namespace MPG {

  class IntPairVar : public VarImpVar<MPG::IntPair::IntPairVarImp> {
  protected:
    using VarImpVar<IntPair::IntPairVarImp>::x;
  public:
    IntPairVar(void) {}
    IntPairVar(const IntPairVar& y)
      : VarImpVar<IntPair::IntPairVarImp>(y.varimp()) {}
    IntPairVar(IntPair::IntPairVarImp* y)
      : VarImpVar<IntPair::IntPairVarImp>(y) {}
    // variable creation
    IntPairVar(Space& home, int xmin,int ymin, int xmax, int ymax)
      : VarImpVar<IntPair::IntPairVarImp>
	(new (home) IntPair::IntPairVarImp(home,xmin, ymin, xmax, ymax)) {
      //            if ((min < Int::Limits::min) || (max > Int::Limits::max))
      //  throw Int::OutOfLimits("IntVar::IntVar");
      //if (min > max)
      //throw Int::VariableEmptyDomain("IntVar::IntVar"); 
    }
    // access operations
    
    IntPair::Pair min(void) const {
      return x->min();
    }

    IntPair::Pair max(void) const {
      return x->max();
    }
    /*
    int max(void) const {
      return x->max();
      }
    */



  };
  
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const IntPairVar& x) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    if (x.assigned())
      s << x.min();
    else
      s << '[' << x.min() <<  ".." << x.max() << ']';
    return os << s.str();
  }

}




/*
// array traits
namespace MPG {
  class IntVarArgs; class IntVarArray;
}

namespace Gecode {

  template<>
  class ArrayTraits<Gecode::VarArray<MPG::IntVar> > {
  public:
    typedef MPG::IntVarArray  StorageType;
    typedef MPG::IntVar       ValueType;
    typedef MPG::IntVarArgs   ArgsType;
  };
  template<>
  class ArrayTraits<MPG::IntVarArray> {
  public:
    typedef MPG::IntVarArray  StorageType;
    typedef MPG::IntVar       ValueType;
    typedef MPG::IntVarArgs   ArgsType;
  };
  template<>
  class ArrayTraits<Gecode::VarArgArray<MPG::IntVar> > {
  public:
    typedef MPG::IntVarArgs   StorageType;
    typedef MPG::IntVar       ValueType;
    typedef MPG::IntVarArgs   ArgsType;
  };
  template<>
  class ArrayTraits<MPG::IntVarArgs> {
  public:
    typedef MPG::IntVarArgs  StorageType;
    typedef MPG::IntVar      ValueType;
    typedef MPG::IntVarArgs  ArgsType;
  };

}
// variable arrays
namespace MPG {

  class IntVarArgs : public VarArgArray<IntVar> {
  public:
    IntVarArgs(void) {}
    explicit IntVarArgs(int n) : VarArgArray<IntVar>(n) {}
    IntVarArgs(const IntVarArgs& a) : VarArgArray<IntVar>(a) {}
    IntVarArgs(const VarArray<IntVar>& a) : VarArgArray<IntVar>(a) {}
    IntVarArgs(Space& home, int n, int min, int max)
      : VarArgArray<IntVar>(n) {
      for (int i=0; i<n; i++)
        (*this)[i] = IntVar(home,min,max);
    }
  };

  class IntVarArray : public VarArray<IntVar> {
  public:
    IntVarArray(void) {}
    IntVarArray(const IntVarArray& a)
      : VarArray<IntVar>(a) {}
    IntVarArray(Space& home, int n, int min, int max)
      : VarArray<IntVar>(home,n) {
      for (int i=0; i<n; i++)
        (*this)[i] = IntVar(home,min,max);
    }
  };

}
*/



#endif