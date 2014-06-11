
// Foo feels like a class... this is strange.
#pragma acre ignore
struct Foo {
  private:
    int Data;
  public:
    Foo() : Data(0) { }
    int getData() const { return Data; }
    void setData(int D) { Data = D; }
  protected:
    Foo(int a) : Data(a) { }
};

// Violation #1 (template struct)
#pragma acre ignore
template<typename T>
struct Bar {
  private:
    int Data;
  public:
    Bar() : Data(0) { }
};

// Violation #2
#pragma acre ignore scope
struct FooBar {
  protected:
    int Data;
  public:
    FooBar() : Data(0) { }
};

class Baz {
  public:
    Bar<int> obj;
    int Data;
    Baz() : Data(0) { }
};
