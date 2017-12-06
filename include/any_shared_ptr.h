// MIT License
//
// Copyright (c) 2017 Scott Slack-Smith
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once
#include <memory>
#include <typeinfo>
#include <type_traits>
#ifdef _MSC_VER
  #if _HAS_CXX17 != 0
    #include <optional>
    #define ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
  #endif
#else // GCC
  #if __has_include(<optional>) // requires GCC 5 or greater
    #include <optional>
    #define ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
  #else
    #include <experimental/optional>
    namespace std {
      using std::experimental::optional;
    } // namespace std
    #define ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
  #endif
#endif

namespace xxx {

  class bad_any_shared_ptr_cast : public std::bad_cast
  {
  public:
    virtual const char* what() const noexcept override { return "bad any_shared_ptr cast"; }
  };

  inline namespace v1 {

    /** 
    The class any_shared_ptr is type-safe container for any std::shared_ptr<T>
    that, unlike std::any, supports cv-qualifier promotion and dynamic up-casting.
    For example:

      struct Base {};
      struct Derived : public Base {};

      // *** std::any ***
      { 
        any a{ make_shared<Derived>() };
        any_cast< shared_ptr<Derived      > >( a ); // cast to same type succeeds
        any_cast< shared_ptr<const Derived> >( a ); // CAST FAILS - no cv-qualifier promotion
        any_cast< Base                      >( a ); // CAST FAILS - no dynamic up-cast
      }

      // *** xxx::any_shared_ptr ***
      {
        xxx::any_shared_ptr a{ make_shared<Derived>() };
        xxx::any_shared_ptr_cast< Derived       >( a ); // cast to same type succeeds
        xxx::any_shared_ptr_cast< const Derived >( a ); // cv-qualifier promotion succeeds
        xxx::any_shared_ptr_cast< Base          >( a ); // dynamic up cast succeeds
      }
    */
    class any_shared_ptr
    {
    public:
      //-----------------------------------------------------
      // Canonical "Rule of 5" ctors/dtor and copy operators

      ~any_shared_ptr() = default;

      any_shared_ptr(any_shared_ptr const & other) noexcept = default;

      any_shared_ptr(any_shared_ptr && other) noexcept = default;

      any_shared_ptr& operator=(any_shared_ptr const& other) noexcept = default;

      any_shared_ptr& operator=(any_shared_ptr && other) noexcept = default;

      //-----------------------------------------------------
      // ctors

      constexpr any_shared_ptr() noexcept = default;

      template<typename T>
      any_shared_ptr(std::shared_ptr<T> ptr) noexcept
        : my_type_info{ &typeid(HeldType<T>) }
        , my_shared_ptr{ std::const_pointer_cast<std::remove_cv_t<T>>(std::move(ptr)) }
        , my_throw_func{ &any_shared_ptr::throw_func<T> }
      {
      }

      //-----------------------------------------------------
      // Modifiers

      // swaps two any objects
      void swap(any_shared_ptr & other) noexcept { other = std::exchange(*this, std::move(other)); }

      // reset to empty state 
      void reset() noexcept { ::new (this) any_shared_ptr(); }

      //-----------------------------------------------------
      // Observers

      // return true if not empty
      bool has_value() const noexcept { return my_throw_func != nullptr; }

      // Returns typeid(std::shared_ptr<T>) if instance is non-empty,
      // otherwise typeid(void).
      const std::type_info & type() const noexcept { return *my_type_info; }

      // returns the number of shared_ptr objects referring to the same managed object 
      long  use_count() const noexcept { return my_shared_ptr.use_count(); }

    private:

      // Make alias for signature for throw function  
      using Throw_func = void(void *);

      // Holds typeid(shared_ptr<T*) if instance is not empty, 
      // otherwise set to typeid(void) to indicate an empty state.
      const std::type_info *  my_type_info{ &typeid(void) };
      // The held shared_ptr, 
      std::shared_ptr<void>   my_shared_ptr{ nullptr };
      // The throw function that implements a dynamic up cast
      Throw_func *            my_throw_func{ nullptr };

      template<typename T>
      using HeldType = std::shared_ptr<T>;

      // Replicate a dynamic up cast by the using try/catch mechanism
      template <typename T>
      std::shared_ptr<T> dynamic_up_cast(bool & cast_ok) const noexcept
      {
        std::shared_ptr<T> result;
        if (has_value()) {
          if (type() == typeid(HeldType<T>)) { // is the same type
            result = std::static_pointer_cast<T>(my_shared_ptr);
            cast_ok = true;
          }
          else { // try an up-cast by throwing an exception
            try {
              my_throw_func(my_shared_ptr.get());
            }
            catch (T* p) { // up-cast succeeded
              result = std::shared_ptr<T>(my_shared_ptr, p);
              cast_ok = true;
            }
            catch (...) { // up-cast failed
            }
          }
        }
        return result;
      }

      // Throw exception to implement implicit up cast 
      template<typename T>
      static void throw_func(void * ptr)
      {
        throw static_cast<T*>(ptr);
      }

      template<typename T>
      friend std::shared_ptr<T> any_shared_ptr_cast(any_shared_ptr const & anySharedPtr);

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL
      template<typename T>
      friend std::optional<std::shared_ptr<T>> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept;
#else // replace std::optional<std::shared_ptr<T>> with std::pair<std::shared_ptr<T>,bool>
      template<typename T>
      std::pair<std::shared_ptr<T>, bool> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept;
#endif

    };

    //-----------------------------------------------------------------------------------------------------

    template<typename T>
    std::shared_ptr<T> any_shared_ptr_cast(any_shared_ptr const & anySharedPtr)
    {
      bool is_cast_ok{ false };
      const std::shared_ptr<T> result(anySharedPtr.template dynamic_up_cast<T>(is_cast_ok));
      if (is_cast_ok) {
        return result;
      }
      throw bad_any_shared_ptr_cast();
    }

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL

    template<typename T>
    std::optional<std::shared_ptr<T>> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept
    {
      std::optional<std::shared_ptr<T>> result;
      bool is_cast_ok{ false };
      const std::shared_ptr<T> cast_result = anySharedPtr->template dynamic_up_cast<T>(is_cast_ok);
      if (is_cast_ok) {
        result = cast_result;
      }
      return result;
    }

#else // replace std::optional<std::shared_ptr<T>> with std::pair<std::shared_ptr<T>,bool>

    template<typename T>
    std::pair<std::shared_ptr<T>, bool> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept
    {
      bool is_cast_ok{ false };
      const std::shared_ptr<T> cast_result = anySharedPtr->template dynamic_up_cast<T>(is_cast_ok);
      return std::make_pair(cast_result, is_cast_ok);
    }

#endif

    // Constructs an any object containing an object of type shared_ptr<T>, 
    // passing the provided arguments to std::make_shared<T>.
    // Is equivalent to 
    //    return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) }
    template<class T, class... Args>
    inline   any_shared_ptr make_any_shared_ptr(Args&&... args)
    {
      return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) };
    }

  } // namespace v1


  namespace v2 {

    /**
    The class v2::any_shared_ptr is more performant implementation 
    of v1::any_shared_ptr.

    It's more performant because;
      1. sizeof(v2::any_shared_ptr) < sizeof(v1::any_shared_ptr) by 1 pointer
      2. the MSVC performance of casting to same type is marginally better
         due to avoiding a share_ptr copy.    
    */
    class any_shared_ptr
    {
    public:
      //-----------------------------------------------------
      // Canonical "Rule of 5" ctors/dtor and copy operators

      ~any_shared_ptr() { holder()->~IHolder(); }

      any_shared_ptr(any_shared_ptr const & other) noexcept { other.holder()->clone(&my_inplace_storage); }

      any_shared_ptr& operator=(any_shared_ptr const& other) noexcept
      {
        if (this != &other) {
          this->~any_shared_ptr();
          other.holder()->clone(&my_inplace_storage);
        }
        // else a self-copy - do nothing
        return *this;
      }

      any_shared_ptr(any_shared_ptr && other) noexcept
      {
        my_inplace_storage = other.my_inplace_storage;
        ::new (&other) any_shared_ptr();
      }

      any_shared_ptr& operator=(any_shared_ptr && other) noexcept
      {
        if (this != &other) {
          this->~any_shared_ptr();
          my_inplace_storage = other.my_inplace_storage;
          ::new (&other) any_shared_ptr();
        }
        // else a self-move - do nothing
        return *this;
      }

      //-----------------------------------------------------
      // ctors

      any_shared_ptr() noexcept { ::new (&my_inplace_storage) EmptyHolder(); }

      template<typename T>
      any_shared_ptr(std::shared_ptr<T> ptr) noexcept { ::new (&my_inplace_storage) Holder<T>(std::move(ptr));  }

      //-----------------------------------------------------
      // Modifiers

      // swaps two any objects 
      void swap(any_shared_ptr & other) noexcept { other = std::exchange(*this, std::move(other)); }

      // reset to empty state 
      void reset() noexcept 
      { 
        this->~any_shared_ptr();
        ::new (this) any_shared_ptr();
      }

      //-----------------------------------------------------
      // Observers

      // return true if not empty
      bool has_value() const noexcept { return holder()->has_value(); }

      // Returns typeid(std::shared_ptr<T>) if instance is non-empty,
      // otherwise typeid(void).
      const std::type_info & type() const noexcept { return holder()->type(); }

      // returns the number of shared_ptr objects referring to the same managed object 
      long  use_count() const noexcept { return holder()->use_count(); }

    private:

      template<typename T>
      using HeldType = std::shared_ptr<T>;

      template <typename T>
      std::shared_ptr<T> dynamic_up_cast(bool & cast_ok) const noexcept
      {
        std::shared_ptr<T> result;
        if (has_value()) {
          const IHolder * pholder{ holder() };
          if (type() == typeid(HeldType<T>)) {
            // [[gsl::suppress(type.2)]] // warning C26491: Don't use static_cast downcasts. A cast from a polymorphic type should use dynamic_cast. (type.2)
            result = static_cast<const Holder<T>*>(pholder)->my_ptr;
            cast_ok = true;
          }
          else { // try an up cast by throwing an exception
            try {
              pholder->throw_held_pointer();
            }
            catch (T* p) { // implicit up cast succeeded
              result = std::static_pointer_cast<T>(pholder->make_shared_ptr_alias(const_cast<std::remove_cv_t<T>*>(p)));
              cast_ok = true;
            }
            catch (...) { // up cast failed
            }
          }
        }
        return result;
      }

      // Base class for the class holds shared_ptr
      struct IHolder
      {
        virtual ~IHolder() = default;

        virtual bool                    has_value() const noexcept = 0;
        virtual const std::type_info&   type() const noexcept = 0;
        virtual long                    use_count() const noexcept = 0;
        virtual const IHolder *         clone(void* const inplaceMemory) const noexcept = 0;
        virtual void                    throw_held_pointer() const = 0;
        virtual std::shared_ptr<void>   make_shared_ptr_alias(void * ptr) const noexcept = 0;
      };

      template<typename T>
      struct Holder final : public IHolder
      {
        // The held shared_ptr
        std::shared_ptr<T>  my_ptr;

        Holder() = default;
        Holder(std::shared_ptr<T> ptr) : my_ptr(std::move(ptr)) {}

        bool                    has_value() const noexcept { return true; }
        const std::type_info&   type() const noexcept { return typeid(HeldType<T>); }
        long                    use_count() const noexcept final { return my_ptr.use_count(); }
        const IHolder *         clone(void* const inplaceMemory) const noexcept final { return ::new (inplaceMemory) Holder(my_ptr); }
        void                    throw_held_pointer() const final { throw my_ptr.get(); }
        std::shared_ptr<void>   make_shared_ptr_alias(void* p) const noexcept final { return std::shared_ptr<void>(my_ptr, p); }
      };

      struct EmptyHolder final : public IHolder
      {
        // Not used - is required so that sizeof(Holder<void>) == sizeof(EmptyHolder)
        std::shared_ptr<void>  my_ptr;

        EmptyHolder() = default;

        bool                    has_value() const noexcept { return false; }
        const std::type_info&   type() const noexcept { return typeid(void); }
        long                    use_count() const noexcept final { return 0; }
        const IHolder *         clone(void* const inplaceMemory) const noexcept final { return ::new (inplaceMemory) EmptyHolder(); }
        void                    throw_held_pointer() const final {}
        std::shared_ptr<void>   make_shared_ptr_alias(void* ) const noexcept final { return my_ptr; }
      };

      static_assert(sizeof(Holder<void>) == sizeof(EmptyHolder), "It's not essential but sizeof(Holder<void>) == sizeof(EmptyHolder)"
                                                                 " is better so that all bytes in my_inplace_storage are initialized");

      using storage_t = typename std::aligned_storage<sizeof(Holder<void>), std::alignment_of<Holder<void>>::value>::type;

      // Inplace storage to hold Holder<T>
      storage_t               my_inplace_storage;

      // Pointer to my_inplace_storage that holds Holder<T> 
      const IHolder *  holder() const noexcept 
      { 
        // [[gsl::suppress(type.1)]]  // warning C26490 : Don't use reinterpret_cast. (type.1)
        return reinterpret_cast<const IHolder *>(&my_inplace_storage); 
      }

      template<typename T>
      friend std::shared_ptr<T> any_shared_ptr_cast(any_shared_ptr const & anySharedPtr);

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL
      template<typename T>
      friend std::optional<std::shared_ptr<T>> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept;
#else // replace std::optional<std::shared_ptr<T>> with std::pair<std::shared_ptr<T>,bool>
      template<typename T>
      std::pair<std::shared_ptr<T>, bool> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept;
#endif

    };

    //-----------------------------------------------------------------------------------------------------

    template<typename T>
    std::shared_ptr<T> any_shared_ptr_cast(any_shared_ptr const & anySharedPtr)
    {
      bool is_cast_ok{ false };
      const std::shared_ptr<T> result = anySharedPtr.template dynamic_up_cast<T>(is_cast_ok);
      if (is_cast_ok) {
        return result;
      }
      throw bad_any_shared_ptr_cast();
    }

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL
    template<typename T>
    std::optional<std::shared_ptr<T>> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept
    {
      std::optional<std::shared_ptr<T>> result;
      bool is_cast_ok{ false };
      const std::shared_ptr<T> cast_result = anySharedPtr->template dynamic_up_cast<T>(is_cast_ok);
      if (is_cast_ok) {
        result = std::move(cast_result);
      }
      return result;
    }
#else // replace std::optional<std::shared_ptr<T>> with std::pair<std::shared_ptr<T>,bool>
    template<typename T>
    std::pair<std::shared_ptr<T>,bool> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept
    {
      bool is_cast_ok{ false };
      const std::shared_ptr<T> cast_result = anySharedPtr->template dynamic_up_cast<T>(is_cast_ok);
      return std::make_pair(cast_result, std::move(is_cast_ok));
    }
#endif

    // Constructs an any object containing an object of type shared_ptr<T>, 
    // passing the provided arguments to std::make_shared<T>.
    // Is equivalent to 
    //    return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) }
    template<class T, class... Args>
    inline   any_shared_ptr make_any_shared_ptr(Args&&... args)
    {
      return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) };
    }

  } // namespace v2

} // namespace xxx {


namespace std {

  inline void swap(xxx::v1::any_shared_ptr & lhs, xxx::v1::any_shared_ptr & rhs)
  {
    lhs.swap(rhs);
  }

  inline void swap(xxx::v2::any_shared_ptr & lhs, xxx::v2::any_shared_ptr & rhs)
  {
    lhs.swap(rhs);
  }

} // namespace std

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
#undef ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
#endif