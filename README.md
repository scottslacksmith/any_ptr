# any_ptr
This project implements 2 complementary C++ classes for ```std::any```

1. ```any_ptr``` - a type-safe container for pointers of any type. 

2. ```any_shared_ptr```- a type-safe container for any std::shared_ptr. 

that, unlike ```std::any```,  preserves the cv-qualifier rules and the implicit up cast behaviour of pointers. 

This implementation is inspired by [[1]](#heading references) where Cassio Neri observes that C++'s try/catch mechanism can be used to implement the desired behaviour of pointers.   

## Motivation

### What's wrong with ```std::any``` holding std::shared_ptr
Consider
```
struct Base {
};

struct Derived : public Base {
};

...
using namespace std;

any any{ make_shared<Derived>() };

shared_ptr<Base> base = any_cast<std::shared_ptr<Base>>( any );  // THROWS std::bad_any_cast - no implicit upcast

shared_ptr<const Derived> derived = any_cast<shared_ptr<const Derived>>( any );  // THROWS std::bad_any_cast - no implicit cv-qualifier promotion
```

### Why is ```any_shared_ptr``` better 
```
struct Base {
};

struct Derived : public Base {
};

...
using namespace std;

any_share_ptr any{ make_shared<Derived>() };

shared_ptr<Base> base = any_shared_ptr_cast<std::shared_ptr<Base>>( any );  // OK - implicit upcast supported

shared_ptr<const Derived> derived = any_shared_ptr_cast<shared_ptr<const Derived>>( any );  // OK - implicit cv-qualifier promotion supported
```


## The performance penalty
The C++ try/catch mechanism has a well deserved reputation for slow performance. 
Using Google's microbenchmark library (see the src/benchmark folder) we observe that the implicit upcast is ~100x slower than the basic cast to the same type held by ```any_shared_ptr```.

|Benchmark (x64) |MSVC 2017|GCC 6.2|Clang 3.9|
|-|-|-|-|
|Cast|24 ns|15 ns|15 ns|
|Implicit up cast|2250 ns|2040 ns|2086 ns|
|

The processor used for benchmark was an Intel i7-4710HQ 2.3GHz 

## Debug vs Release 
By default, benchmark builds as a debug library. You will see a warning in the output when this is the case. To build it as a release library, use:
```
cmake -DCMAKE_BUILD_TYPE=Release
```
To enable link-time optimisation, use
```
cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_LTO=true
```
To use Clang
```
cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_LTO=true -DCMAKE_CXX_COMPILER=clang++-3.9
```

## Compiler Support

```any_shared_ptr``` is implemented using ```shared_ptr```'s aliasing contructor that was added to the C++11 standard and thus was not in the TR1 version of ```shared_ptr``` (see Anthony Williams execellent blog [std::shared_ptr's secret constructor](https://www.justsoftwaresolutions.co.uk/cplusplus/shared-ptr-secret-constructor.html) for further details). In addition Google's ***benchmark*** and ***test*** libraries require a modern C++ toolchain, both compiler and standard library.

The following minimum versions are strongly recommended to build the library:  
* GCC 5  
* Clang 4  
* Visual Studio 2017  

Anything older may work.

## References

[1] Cassio Neri, _Twisting the RTTI System for Safe Dynamic Casts of void* in C++_, [Dr Dobbs](http://www.drdobbs.com/cpp/twisting-the-rtti-system-for-safe-dynami/229401004), (2011).
