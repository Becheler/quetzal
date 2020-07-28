
// compiles with g++ -o test test.cpp -std=c++17 -Wall

#include "../expressive.h"
#include "assert.h"
#include <map>

// identity function of integer
class Foo
{
  std::map<int, int> _values;
public:
  Foo()
  {
    _values[0] = 0;
    _values[1] = 1;
    _values[2] = 2;
  }

  int operator()(int t) const
  {
    return _values.at(t);
  }

};

struct Bar
{

  int operator()(int a, int b) const
  {
    return a + b;
  }

};

int main(){

  using quetzal::expressive::use;

  quetzal::expressive::literal_factory<int,int> lit;
  auto g  = lit(5);

  auto f = [](int a, int b){ return a + b; };
  auto h  = use(f) + g ;
  assert( h(2,3) == 10);


  Bar b;
  auto i = use(b) + g ;
  assert( i(2,3) == 10);


  quetzal::expressive::literal_factory<int> lit2;
  Foo foo;
  assert(foo(2)==2);


  auto j = use(foo);
  assert(j(2)==2);

  auto k = j + lit2(7) ;
  assert( k(1) == 8 );

  return 0;

}
