# any_ptr
This project implements the following 2 complementary C++ classes for C++17's ```std::any```;

1. ```any_ptr``` - a type-safe container for pointers to any type. 

2. ```any_shared_ptr```- a type-safe container for std::shared_ptr\<T\> of any type T. 

that, unlike ```std::any```,  preserves pointer cv-qualifier promotion and implicit upcast behaviour. 

## Why do we need any_shared_ptr

Consider the following trivial example:
```
struct Base {};

struct Derived : public Base {};

std::shared_ptr<Derived> ptr = std::make_shared<Derived>();
```
You can use ```std::any``` to store ```std::shared_ptr<Derived>``` but its pointer semantics are lost. 

```
using namespace std;

any any{ ptr };

// OK  - casting to the same cv-qualified type
shared_ptr<Derived> derived = any_cast< shared_ptr<Derived> >( any );  

// FAILED - throws std::bad_any_cast as there's no implicit cv-qualifier promotion
shared_ptr<const Derived> const_derived = any_cast< shared_ptr<const Derived> >( any );  

// FAILED - throws std::bad_any_cast as there's no implicit upcast
shared_ptr<Base> base = any_cast< shared_ptr<Base> >( any );
```
Now consider ```any_shared_ptr```
```
using namespace std;

any_share_ptr any{ ptr };

// OK  - casting to the same cv-qualified type
shared_ptr<Derived> derived = any_shared_ptr_cast< Derived >( any );  

// OK - implicit cv-qualifier promotion is supported
shared_ptr<const Derived> const_derived = any_shared_ptr_cast< const Derived >( any );  

// OK - implicit upcast is supported
shared_ptr<Base> base = any_shared_ptr_cast< Base >( any );  
```
and ```any_ptr```.
```
using namespace std;

Derived * p = ptr.get(); 

any_ptr any{ p };

// OK  - casting to the same cv-qualified type
Derived* derived = any_ptr_cast< Derived >( any );  

// OK - implicit cv-qualifier promotion is supported
const Derived* const_derived = any_ptr_cast< const Derived >( any );  

// OK - implicit upcast is supported
Base* base = any_ptr_cast< Base >( any );  
```
Many causal users of ```std::any``` may be surprised to find that ```std::any``` doesn't preserve pointer cv-qualifier promotion or implicit up cast behaviour. This is not a ```std::any``` defect. Its primary purpose is to store *objects* and not *references* to an object. In contrast ```any_ptr```/```any_shared_ptr``` are designed to store *references* to an object and thus preserves normal pointer behaviour. However there's a catch if performance is critical.
## What's the catch
To implement ```any_ptr```/```any_shared_ptr``` we need a new function, let's call it dynamic_up_cast, that's similar to C++'s dynamic_cast except that it only performs an up cast. We could try implementing dynamic_up_cast by accessing the compilers internal RTTI data structures in a similar manner as dynamic_cast. However many compiler/platform combinations would require its own implementation which is not very appealing. Instead a better solution is a portable implementation that needs nothing more the C++ standard. The inspiration we need is Cassio Neri's observation [[1]](#references) that throwing and catching an exception can be use to implement an up cast as shown in the following code snippet.    

```
struct any_ptr
{ 
  using Throw_pointer_func = void(void *);

  Throw_pointer_func *    my_throw_pointer_func;
  void *                  my_ptr;

  template<typename T>
  static void throw_pointer(void * ptr)
  {
    throw static_cast<T*>(ptr);
  }

  template<typename T>
  any_ptr::any_ptr(T* ptr)
    : my_throw_pointer_func{ & any_ptr::throw_pointer<T> }
    , my_ptr{ const_cast<T*>(ptr) }
  {
    assert(ptr!=nullptr);
  }

  template<typename CastToType>
  CastToType* dynamic_up_cast()
  {
    try {
      my_throw_pointer_func(my_ptr);
    }
    catch(CastToType* to_ptr) {
      // note my_ptr != to_ptr is possible for 
      // class hierarchies using multiple inheritance 
      return to_ptr;
    }
    return nullptr;
  }
};

template<typename CastToType>
CastToType* any_ptr_cast(any_ptr const & a)
{
  CastToType* ptr = a.template dynamic_up_cast<CastToType>();
  if (ptr) {
    return ptr;
  }
  throw std::bad_cast();
}
```

### The performance penalty
C++ exceptions are intended to used as an error reporting mechanism and thus the performance of try/catch blocks are optimised for the situation when no exception is thrown. Consequently we can expect a performance penalty if our normal non-error code path is based on throwing and catching an exception. 

Using Google's microbenchmark library (see the src/benchmark folder) we observe that the implicit upcast is ~100x slower than the basic cast to the same type held by ```any_shared_ptr```.

||Benchmark (x64) |MSVC 2017|GCC 6.2|Clang 3.9|
|-|-|-|-|-|
|**any_shared_ptr**|||||
|any_shared_ptr_cast< Derived >( any )|Cast to same type|27 ns|15 ns|15 ns|
|any_shared_ptr_cast< Base >( any )|Up cast|2232 ns|2040 ns|2086 ns|
|**any_ptr**|||||
|any_ptr_cast< Derived >( any )|Cast to same type|5 ns|2 ns|2 ns|
|any_ptr_cast< Base >( any )|Up cast|2200 ns|1950 ns|1950 ns|
|

The processor used for benchmark was an Intel i7-4710HQ 2.3GHz 

## Debug vs Release 
By default, any_ptr builds as a debug library. You will see a warning in the output when this is the case. To build it as a release library, use:
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
