#ifndef __MPG_INTPAIRAPPROXVARIMP_HH__
#define __MPG_INTPAIRAPPROXVARIMP_HH__

#include <gecode/kernel.hh>

#include <iostream>
#include <vector>
#include <assert.h>
#include <gecode/int.hh>

using Gecode::Space;
using Gecode::ModEvent;
using Gecode::Propagator;
using Gecode::PropCond;
using Gecode::Archive;
using Gecode::Delta;
using namespace Gecode;

#include "../common/pair.hh"
#include "../common/dummydelta.hh"
 

struct PairApprox {
  int x, l, h;
  PairApprox() {};
  PairApprox(const PairApprox& p) : x(p.x), l(p.l), h(p.h) {};
  PairApprox(int x, int l, int h) : x(x), l(l), h(h) {};
}


class IntPairApproxVarImp : public IntPairApproxVarImpBase {
protected:
    std::vector<PairApprox> domain;

  
public:
    IntPairApproxVarImp(Space& home, int x_min, int x_max, int y_min, int y_max)
        : IntPairApproxVarImpBase(home) {
        assert(x_min<=x_max && y_min<=y_max);
        for(int x=x_min; x<=x_max; x++)
	  domain.push_back(PairApprox(x,y_min, y_max));
    }
    // For printouts and brancher
    Pair first() const {
        assert(domain.size()>0);
        return Pair(domain[0].x, domain[0].l);
    }

    Pair last() const {
        assert(domain.size()>0);
        return Pair(domain.back().x, domain.back().h);
    }

    // access operations
    int xmax(void) const {
        assert(domain.size()>0);
        return domain.back().x;
    }

    int xmin(void) const {
        assert(domain.size()>0);
        return domain[0].x;
    }

    bool contains(const Pair& p) const {
        assert(domain.size()>0);
        for(int i=0; i<domain.size(); i++)
            if(p.x == domain[i].x && p.y>=domain[i].l && py <=domain[i].h)
                return true;
        return false;
    }

  Pair val() const {
        assert(assigned());
        return Pair(domain[0].x, domain[0].l);
    }

  int getxindex(int x) {
    for(int i=0; i<domain.size(); i)
      if(domain[i].x==x)
	return i;
    return -1;
  }
 
  // Needs rewriting
  //  Pair getElement(int n) {
  //    assert(n<domain.size());
  //    return Pair(domain[n]);
  //  }

    // For cost computation and printout
    int size(void) const {
      return domain.size(); // Not correct, but not important
    }

    // assignment test
    bool assigned(void) const {
        return domain.size()==1;
    }

    // Mod events
    enum IP_INT_REL { IP_LQ, IP_LT, IP_GQ, IP_GT, IP_EQ, IP_NQ };


    ModEvent rel(Space &home, int dim, int n, IP_INT_REL r);


    ModEvent lq(Space& home, int dim, int n) { return rel(home, dim, n, IP_LQ); }
    ModEvent lt(Space& home, int dim, int n) { return rel(home, dim, n, IP_LT); }
    ModEvent gq(Space& home, int dim, int n) { return rel(home, dim, n, IP_GQ); }
    ModEvent gt(Space& home, int dim, int n) { return rel(home, dim, n, IP_GT); }
    ModEvent eq(Space& home, int dim, int n) { return rel(home, dim, n, IP_EQ); }
    ModEvent nq(Space& home, int dim, int n) { return rel(home, dim, n, IP_NQ); }


    ModEvent xlq(Space& home, int n);
    ModEvent eq(Space& home, const Pair& p);
    ModEvent eq(Space& home, const IntPairApproxVarImp& p);

    ModEvent eq(Space& home, const std::vector<Pair> & v);

    ModEvent nq(Space& home, const Pair& p);

    // Subscriptions and cancel
    void subscribe(Space& home, Propagator & prop, PropCond pc, bool schedule = true) {
      //        std::cout << "Subscribing from varimp" << std::endl;
        IntPairApproxVarImpBase::subscribe(home, prop, pc, assigned(), schedule);
    }

    void cancel(Space& home, Propagator& prop, PropCond pc) {
        IntPairApproxVarImpBase::cancel(home, prop, pc, assigned());
    }

    IntPairApproxVarImp(Space& home, bool share, IntPairApproxVarImp& y)
        : IntPairApproxVarImpBase(home,share,y), domain(y.domain) {}
    IntPairApproxVarImp* copy(Space& home, bool share) {
        if (copied())
            return static_cast<IntPairApproxVarImp*>(forward());
        else
            return new (home) IntPairApproxVarImp(home,share,*this);
    }

};

template<class Char, class Traits>
std::basic_ostream<Char,Traits>&
operator <<(std::basic_ostream<Char,Traits>& os, const IntPairApproxVarImp& x) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    if (x.assigned())
        s << x.first();
    else
        s << '[' << x.first() <<  ".." << x.last() << "][" << x.size() << "]";
    return os << s.str();
}



ModEvent IntPairApproxVarImp::rel(Space&home, int dim, int n, IP_INT_REL r)
{
    // This is very inefficient with the current representation.
    // It would be much better with vector<vector<int>> istead
    // of vector<Pair>
    bool modified=false;

    for(int i=0; i<domain.size(); i++) {
        //	  std::cout << "Test " << domain[i] << std::endl;
        bool erase = false;

        if(dim==0) {
            ptr=&domain[i].x;
	    int *ptr;

	    switch (r) {
            case IP_LQ : if(*ptr > n) erase = true; break;
            case IP_LT : if(*ptr >= n) erase = true; break;
            case IP_GQ : if(*ptr < n) erase = true; break;
            case IP_GT : if(*ptr <= n) erase = true; break;
            case IP_EQ : if(*ptr != n) erase = true; break;
            case IP_NQ : if(*ptr == n) erase = true; break;
	    default : break;
	    }
	}
	else if(dim==1) {
	  int *l, *h;
	  l=&domain[i].l; h=&domain[i].h;
	  
	    switch (r) {
            case IP_LQ : if(*h > n) { *h = n; modified = true; }break;
            case IP_LT : if(*h >= n) { *h=n-1; modified = true; } break;
            case IP_GQ : if(*l < n) { *l = n; modified=true; } break;
            case IP_GT : if(*l <= n) { *l = n+1; modified = true; } break;
            case IP_EQ : if(*l > n || *h < n) erase = true; else { *l=*h=n; modified=true;} break;
            case IP_NQ : if(*l == n) {*n++; modified=true; } if(*h == n) { *h--; modified=true; } break;
	    default : break;
	    }
	    if(*l>*h)
	      erase=true;
	}
	if (erase) {
	  //	    std::cout << "Erase " << domain[i] << std::endl;
	  modified=true;
	  domain.erase(domain.begin() + i);
	  i--;
	}

    }
    if (!modified)
      return ME_INTPAIR_NONE;
    else if (domain.size()==0)
      return ME_INTPAIR_FAILED;
    DummyDelta d;
    return notify(home, assigned() ? ME_INTPAIR_VAL : ME_INTPAIR_DOM, d);
}



ModEvent IntPairApproxVarImp::eq(Space& home, const IntPairApproxVarImp& p) {
    // Probably the most inefficient in the universe. TODO
//    std::cout << "Eq pair pair" << std::endl;
    bool modified=false;
//    std::cout << "Erase " << domain.size() << " " << p.domain.size() << std::endl;
    for(int i=0; i<domain.size(); i++) {
      int index=p.getxindex(domain[i].x);
      if(index==-1)
	domain.erase(domain.begin()+i); i--; modified=true;
      else {
	if(domain[i].l < p.domain[index].l) {
	  domain[i].l=p.domain[index].l; modified = true;
	}
	if(domain[i].h > p.domain[index].h) {
	  domain[i].h=p.domain[index].h; modified = true;
	}
      } 
    }


    if (!modified)
      return ME_INTPAIR_NONE;
    else if (domain.size()==0)
      return ME_INTPAIR_FAILED;
    DummyDelta d;
    return notify(home, assigned() ? ME_INTPAIR_VAL : ME_INTPAIR_DOM, d);
}

ModEvent IntPairApproxVarImp::eq(Space& home, const std::vector<Pair> & v) {
    // Probably the most inefficient in the universe. TODO
//    std::cout << "Eq vector" << std::endl;
    bool modified=false;
//    std::cout << "Erase " << domain.size() << " " << v.size() << std::endl;
    for(int i=0; i<domain.size(); i++)
        for(int j=0; j<v.size(); j++)
            if(domain[i] == v[j]) {
                domain.erase(domain.begin()+i); i--;
                modified=true;
            }


    if (!modified)
      return ME_INTPAIR_NONE;
    else if (domain.size()==0)
      return ME_INTPAIR_FAILED;
    DummyDelta d;
    return notify(home, assigned() ? ME_INTPAIR_VAL : ME_INTPAIR_DOM, d);
}



ModEvent IntPairApproxVarImp::eq(Space& home, const Pair& p)
{
    // Ugly and slow as fuck! Rewrite! TODO
    bool modified = false;
    //    std::cout << "VarImp::Eq" << std::endl;
    for(int i=0; i<domain.size(); i++) {
//        std::cout << "Eq trying " << p << " and " << domain[i] << std::endl;

        if (domain[i].x!=p.x || domain[i].y!=p.y) {
  //          std::cout << "Eq erase " << domain[i] << std::endl;
            domain.erase(domain.begin() + i); i--;
            modified=true;

        }
    }


    if (!modified)
      return ME_INTPAIR_NONE;
    else if (domain.size()==0)
      return ME_INTPAIR_FAILED;
    DummyDelta d;
    return notify(home, assigned() ? ME_INTPAIR_VAL : ME_INTPAIR_DOM, d);
}

ModEvent IntPairApproxVarImp::nq(Space& home, const Pair& p)
{
    // Ugly and slow as fuck! Rewrite! TODO
    bool modified = false;
//    std::cout << "Neq before " << *this << std::endl;
    for(int i=0; i<domain.size(); i++) {
   //     std::cout << "Neq trying " << p << " and " << domain[i] << std::endl;
        if (domain[i].x==p.x && domain[i].y==p.y) {
 //           std::cout << "Neq erase " << domain[i] << std::endl;
            domain.erase(domain.begin() + i); i--;
            modified=true;
        }
    }
//    std::cout << "Neq after " << *this << std::endl;

    if (!modified)
      return ME_INTPAIR_NONE;
    else if (domain.size()==0)
      return ME_INTPAIR_FAILED;
    DummyDelta d;
    return notify(home, assigned() ? ME_INTPAIR_VAL : ME_INTPAIR_DOM, d);
}



ModEvent IntPairApproxVarImp::xlq(Space& home, int n) {
    return lq(home, 0, n);
}

}}
#endif

