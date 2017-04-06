# any_ptr
This project implements the following 2 complementary C++ classes for C++17's ```std::any```;

1. ```any_ptr``` - a type-safe container for pointers to any type. 

2. ```any_shared_ptr```- a type-safe container for std::shared_ptr<T> of any type T. 

that, unlike ```std::any```,  preserves the cv-qualifier promotion rules and the implicit up cast behaviour of pointers. 

## Why do we need any_shared_ptr

Consider
```
struct Base {};

struct Derived : public Base {};

std::shared_ptr<Derived> ptr = std::make_shared<Derived>();
```
```std::any``` can quite happily store ```std::shared_ptr<Derived>``` but its pointer sematics are lost. 

```
using namespace std;

any any{ ptr };

// OK when casting to the same cv-qualified type
shared_ptr<Derived> derived = any_cast<shared_ptr<Derived>>( any );  

// THROWS std::bad_any_cast - no implicit cv-qualifier promotion
shared_ptr<const Derived> derived = any_cast<shared_ptr<const Derived>>( any );  

// THROWS std::bad_any_cast - no implicit upcast
shared_ptr<Base> base = any_cast<shared_ptr<Base>>( any );
```
Many causal users of ```std::any``` may be surprised to find that ```std::any``` doesn't preserve pointer> cv-qualifier promotion or implicit up cast behaviour. This not a ```std::any``` defect. It's specifically designed to store objects and not references to an object. 
```
using namespace std;

any_share_ptr any{ ptr };

// OK when casting to the same cv-qualified type
shared_ptr<Derived> derived = any_shared_ptr_cast<shared_ptr<Derived>>( any );  

// OK - implicit upcast is supported by any_share_ptr
shared_ptr<Base> base = any_shared_ptr_cast<std::shared_ptr<Base>>( any );  

// OK - implicit cv-qualifier promotion ia supported by any_shaared_ptr
shared_ptr<const Derived> derived = any_shared_ptr_cast<shared_ptr<const Derived>>( any );  
```
In contrast ```any_shared_ptr``` is designed to store references to objects and thus preserves normal pointer behaviour but there's is performance cost.
## What's the catch

```any_shared_ptr```/```any_ptr``` can recover pointer semantics by using C++'s try/catch mechanism as observed by Cassio Neri (see [1](#heading references)).   

### The performance penalty
C++ exceptions are intended to used as an error reporting mechanism and thus the performance of try/catch is optimised for the situation when no exception is thrown. Consequently we can expect a performance penalty if our implementation is based on throwing and catching exceptions. Using Google's microbenchmark library (see the src/benchmark folder) we observe that the implicit upcast is ~100x slower than the basic cast to the same type held by ```any_shared_ptr```.

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

# References

[1] Cassio Neri, _Twisting the RTTI System for Safe Dynamic Casts of void* in C++_, [Dr Dobbs](http://www.drdobbs.com/cpp/twisting-the-rtti-system-for-safe-dynami/229401004), (2011).
