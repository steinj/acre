
struct NoConStruct {};

class NoConClass {};

namespace Inner {
  struct g
  {
      g(){}
  };
  g one;  // violation #1
}

extern int integerGlobal;  // OK, just an int
Inner::g classGlobal;  // violation #2
NoConStruct c1;   // violation #3
NoConClass c2;    // violation #4

int main() {
  int integerLocal;
  Inner::g classLocal;

  c1;
}
