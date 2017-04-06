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

  inline namespace ver_1 {

    class any_shared_ptr
    {
    public:
      //-----------------------------------------------------
      // Canonical "Rule of 5" ctors/dtor and copy operators

      ~any_shared_ptr();

      any_shared_ptr(any_shared_ptr const & other) noexcept;

      any_shared_ptr(any_shared_ptr && other) noexcept;

      any_shared_ptr& operator=(any_shared_ptr const& other) noexcept;

      any_shared_ptr& operator=(any_shared_ptr && other) noexcept;

      //-----------------------------------------------------
      // ctors

      template<typename T>
      any_shared_ptr(std::shared_ptr<T> ptr) noexcept;

      any_shared_ptr() noexcept;

      //-----------------------------------------------------
      // Modifiers

      void swap(any_shared_ptr & other) noexcept;

      // reset to empty state 
      void reset() noexcept { ::new (this) any_shared_ptr(); }

      //-----------------------------------------------------
      // Observers

      // return true if not empty
      bool has_value() const noexcept { return my_type_info != nullptr; }

      // Return the T's type-info for the held by shared_ptr<T>
      const std::type_info & type() const noexcept;

      // Return true if the held shared_ptr use_count is 1 (mimics std::shared_ptr::unique())
      bool  unique() const noexcept;

    private:

      template<typename T>
      using HeldBaseType = T;

      template<typename T>
      using HeldType = std::shared_ptr<T>;


      template <typename T>
      std::shared_ptr<T> dynamic_up_cast(bool & cast_ok) const noexcept;

      // Base class for the class holds shared_ptr
      struct IHolder
      {
        virtual ~IHolder() = default;

        virtual bool                    unique() const noexcept = 0;
        virtual const IHolder *         clone(void* const inplaceMemory) const noexcept = 0;
        virtual void                    try_up_cast_by_throwing() const = 0;
        virtual std::shared_ptr<void>   make_shared_ptr_alias(void * ptr) const noexcept = 0;
      };


      template<typename T>
      struct Holder final : public IHolder
      {
        Holder()
          : my_ptr()
        {}

        Holder(std::shared_ptr<T> ptr)
          : my_ptr(std::move(ptr))
        {}

        bool                    unique() const noexcept final { return my_ptr.unique(); }

        const IHolder *         clone(void* const inplaceMemory) const noexcept final { return ::new (inplaceMemory) Holder(my_ptr); }

        void                    try_up_cast_by_throwing() const final { throw my_ptr.get(); }

        std::shared_ptr<void>   make_shared_ptr_alias(void* p) const noexcept final 
        { 
          return std::shared_ptr<void>(my_ptr, p);  
        }

        // The held shared_ptr
        std::shared_ptr<T>  my_ptr;

      };

      using storage_t = typename std::aligned_storage<sizeof(Holder<void>), std::alignment_of<Holder<void>>::value>::type;

      const std::type_info *  my_type_info{ nullptr };
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
    // Implementation

    template<typename T>
    any_shared_ptr::any_shared_ptr(std::shared_ptr<T> ptr) noexcept
      : my_type_info( & typeid(HeldType<T>) )
    {
      ::new (&my_inplace_storage) Holder<T>(std::move(ptr));
    }

    // [[gsl::suppress(type.6)]] //  warning C26495 : Variable 'xxx::ver_1::any_shared_ptr::my_inplace_storage' is uninitialized.Always initialize a member variable. (type.6)
    inline any_shared_ptr::any_shared_ptr() noexcept
    {
      ::new (&my_inplace_storage) Holder<void>();
    }

    inline any_shared_ptr::~any_shared_ptr()
    {
      holder()->~IHolder();
    }

    inline any_shared_ptr::any_shared_ptr(any_shared_ptr const & other) noexcept
      : my_type_info( other.my_type_info )
    {
      other.holder()->clone(&my_inplace_storage);
    }

    inline any_shared_ptr::any_shared_ptr(any_shared_ptr && other) noexcept
      : my_type_info(other.my_type_info)
    {
      my_inplace_storage = other.my_inplace_storage;
      other.reset();
    }

    inline any_shared_ptr& any_shared_ptr::operator=(any_shared_ptr const& other) noexcept
    {
      if (this != &other)
      {
        this->~any_shared_ptr();
        my_type_info = other.my_type_info;
        other.holder()->clone(&my_inplace_storage);
      }
      return *this;
    }

    inline any_shared_ptr& any_shared_ptr::operator=(any_shared_ptr && other) noexcept
    {
      if (this != &other)
      {
        this->~any_shared_ptr();
        my_type_info = other.my_type_info;
        my_inplace_storage = other.my_inplace_storage;
        other.reset();
      }
      return *this;
    }

    inline void any_shared_ptr::swap(any_shared_ptr & other) noexcept
    {
      other = std::exchange(*this, std::move(other));
    }

    inline const std::type_info & any_shared_ptr::type() const noexcept
    {
      return my_type_info ? *my_type_info : typeid(void);
    }

    inline bool  any_shared_ptr::unique() const noexcept
    {
      return holder()->unique();
    }

    template <typename T>
    std::shared_ptr<T> any_shared_ptr::dynamic_up_cast(bool & cast_ok) const noexcept
    {
      std::shared_ptr<T> result;
      if (has_value()) {
        const IHolder * pholder{ holder() };
        if (type() == typeid(HeldType<T>)) {
          // [[gsl::suppress(type.2)]] // warning C26491: Don't use static_cast downcasts. A cast from a polymorphic type should use dynamic_cast. (type.2)
          result = static_cast<const Holder<T>*>(pholder)->my_ptr;
          cast_ok = true;
        }
        else { // try an implicit up cast by throwing an exception
          try {
            pholder->try_up_cast_by_throwing();
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
        result = cast_result;
      }
      return result;
    }

#else // replace std::optional<std::shared_ptr<T>> with std::pair<std::shared_ptr<T>,bool>

    template<typename T>
    std::pair<std::shared_ptr<T>,bool> any_shared_ptr_cast(any_shared_ptr const * anySharedPtr) noexcept
    {
      bool is_cast_ok{ false };
      const std::shared_ptr<T> cast_result = anySharedPtr->template dynamic_up_cast<T>(is_cast_ok);
      return std::make_pair(cast_result, is_cast_ok);
    }

#endif

  } // namespace ver_1

  // Constructs an any object containing an object of type shared_ptr<T>, 
  // passing the provided arguments to std::make_shared<T>.
  // Is equivalent to 
  //    return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) }
  template<class T, class... Args>
  inline   any_shared_ptr make_any_shared_ptr(Args&&... args)
  {
    return any_shared_ptr{ std::make_shared<T>(std::forward<Args>(args)...) };
  }

} // namespace xxx {


namespace std {

  inline void swap(xxx::any_shared_ptr & lhs, xxx::any_shared_ptr & rhs)
  {
    lhs.swap(rhs);
  }

} // namespace std

#ifdef ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
#undef ANY_SHARED_PTR_HAS_LIB_OPTIONAL 
#endif