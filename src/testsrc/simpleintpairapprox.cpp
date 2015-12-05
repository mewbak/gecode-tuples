#include "../intpairapprox.h"

#include "_testbase.cpp"


int noSolutions;

using namespace MPG::IntPair;
using namespace MPG;

class Test : public Script {
public:
  /// The actual problem
  IntPairApproxVar p, q;
  IntPairApproxVarArray a;
  Test(const SizeOptions& opt) : a(*this, 2,0,10,0,10), p(*this, 1,3,0,3), q(*this, 3,3,2,3)
  {
    std::cout << "p: " << p << "  q: " << q << "   a:" << a << std::endl;
    eq(*this, q, p);
    eq(*this, a[0], p);
    eq(*this, a[1], q);
    nonenone(*this, a);
  }

  
  /// Constructor for cloning \a s
#ifndef GC_UPDATE
#define GC_UPDATE(var) var.update(*this, share, s.var)
#endif
  
  Test(bool share, Test& s) : Script(share,s) {
    // To update a variable var use:
    // GC_UPDATE(var)
    GC_UPDATE(a);
    GC_UPDATE(p);
    GC_UPDATE(q);
  }
    
  /// Perform copying during cloning
  virtual Space*
  copy(bool share) {
        return new Test(share,*this);
  }

  /// Print solution (originally, now it's just for updating number of solutions)
  virtual void print(std::ostream& os) const {
    // Strange place to put this, but since this functions is called once for every solution ...
    std::cout << "a[0] " << a[0] << "   a[1]: " << a[1] << std::endl;
    noSolutions++;
  }
};

int main(int argc, char* argv[]) {
    SizeOptions opt("Queens");
    Gecode::VarImpDisposer<IntPairApproxVarImp> disposer;
    opt.solutions(0); // Calculate all solutions
    noSolutions=0;
    //    opt.mode(Gecode::SM_GIST);    
    const int expected_no_solutions = 2;
    
    opt.parse(argc,argv);
    ScriptOutput::run<Test,DFS,SizeOptions>(opt);
    cout << "No solutions: " << noSolutions << endl;
    
    assert (expected_no_solutions == noSolutions);

    cout << "  Ok" << endl;
    return 0;
}

// STATISTICS: example-any

