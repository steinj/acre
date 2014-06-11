
struct NoConStruct {};

class NoConClass {};

#pragma acre ignore recursive
namespace Inner {
  struct g
  {
    public:
      g(){}
      void test() {
        int j;
        if (1)
          return;
        else {}
      }
    private:
      int i;
  };
  g one;  // violation #1
}

extern int integerGlobal;  // OK, just an int
#pragma acre ignore
Inner::g classGlobal;  // violation #2
NoConStruct c1;   // violation #3
NoConClass c2;    // violation #4

int main() {
  int integerLocal;
  Inner::g classLocal;

  c1;
}
