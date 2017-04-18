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
#include <any_shared_ptr.h>
#include <type_traits>

using namespace xxx;
using namespace std;

TEST(any_shared_ptr, empty)
{
  any_shared_ptr any;

  ASSERT_FALSE(any.has_value());
  
  // Check the held type
  ASSERT_EQ(any.type(), typeid(void));

  // Test any_shared_ptr_cast throws bad cast error
  EXPECT_THROW(any_shared_ptr_cast<void>(any), bad_any_shared_ptr_cast);
  EXPECT_THROW(any_shared_ptr_cast<int>(any), bad_any_shared_ptr_cast);
}

TEST(any_shared_ptr, basic_cast)
{
  auto p1 = make_shared<int>(42);
  any_shared_ptr any1{ p1 };

  any_shared_ptr any2{ any1 };

  ASSERT_TRUE(any1.has_value());
  // Check the held type
  ASSERT_EQ(any1.type(), typeid(shared_ptr<int>));
  // Test casting to the held type
  shared_ptr<int> p2 = any_shared_ptr_cast<int>(any1);
  ASSERT_EQ(p1.get(), p2.get());
  ASSERT_EQ(p1.get(), any_shared_ptr_cast<int>(any2).get());
}

TEST(any_shared_ptr, void_nullptr)
{
  // Check that any_shared_ptr can hold shared_ptr<void> 
  std::shared_ptr<void> ptr;
  any_shared_ptr any{ ptr };

  ASSERT_TRUE(any.has_value());
  // Check the held type
  ASSERT_EQ(any.type(), typeid(shared_ptr<void>));
  // Test casting to the held type
  shared_ptr<void> p1 = any_shared_ptr_cast<void>(any);
  ASSERT_EQ(p1, nullptr);
  EXPECT_THROW(any_shared_ptr_cast<int>(any), bad_any_shared_ptr_cast);
}

TEST(any_shared_ptr, cv_promotion_violations)
{
  // Test casting from 'const' to 'non-const' fails
  {
    any_shared_ptr any{ make_shared<const int>(42) };

    ASSERT_EQ(any.type(), typeid(shared_ptr<const int>));

    EXPECT_THROW(any_shared_ptr_cast<int>(any), bad_any_shared_ptr_cast);
    EXPECT_THROW(any_shared_ptr_cast<volatile int>(any), bad_any_shared_ptr_cast);
  }
  // Test casting from 'volatile' to 'non-volatile' fails
  {
    any_shared_ptr any{ make_shared<volatile int>(42) };

    ASSERT_EQ(any.type(), typeid(shared_ptr<volatile int>));

    EXPECT_THROW(any_shared_ptr_cast<int>(any), bad_any_shared_ptr_cast);
    EXPECT_THROW(any_shared_ptr_cast<const int>(any), bad_any_shared_ptr_cast);
  }
  // Test casting from 'const volatile' to 'non-volatile-const' fails
  {
    any_shared_ptr any{ make_shared<const volatile int>(42) };

    ASSERT_EQ(any.type(), typeid(shared_ptr<const volatile int>));

    EXPECT_THROW(any_shared_ptr_cast<int>(any), bad_any_shared_ptr_cast);
    EXPECT_THROW(any_shared_ptr_cast<const int>(any), bad_any_shared_ptr_cast);
    EXPECT_THROW(any_shared_ptr_cast<volatile int>(any), bad_any_shared_ptr_cast);
  }
}


TEST(any_shared_ptr, const_promotion)
{
  // Test non-const -> const & non-const cast
  {
    auto ptr = make_shared<int>(42);
    any_shared_ptr anyPtr{ ptr };

    EXPECT_NO_THROW(any_shared_ptr_cast<int>(anyPtr) );
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int>(anyPtr).get());
    EXPECT_NO_THROW(any_shared_ptr_cast<int const>(anyPtr) );
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int const>(anyPtr).get());

    EXPECT_NO_THROW(any_shared_ptr_cast<int>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int>(anyPtr).get());
    EXPECT_NO_THROW(any_shared_ptr_cast<int const>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int const>(anyPtr).get());

  }
  // Test const -> const & non-const cast
  // Note const -> non-const should throw bad_any_shared_ptr_cast
  {
    auto ptr = make_shared<int const>(42);
    any_shared_ptr anyPtr{ ptr };

    EXPECT_THROW(any_shared_ptr_cast<int>(anyPtr), bad_any_shared_ptr_cast);
    EXPECT_NO_THROW(any_shared_ptr_cast<int const>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int const>(anyPtr).get());

    EXPECT_THROW(any_shared_ptr_cast<int>(anyPtr), bad_any_shared_ptr_cast);
    EXPECT_NO_THROW(any_shared_ptr_cast<int const>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int const>(anyPtr).get());
  }
}

TEST(any_shared_ptr, VolatilePromotion)
{
  // Test non-volatile -> volatile & non-volatile cast
  {
    auto ptr = make_shared<int>(42);
    any_shared_ptr anyPtr{ ptr };

    EXPECT_NO_THROW(any_shared_ptr_cast<int>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int>(anyPtr).get());
    EXPECT_NO_THROW(any_shared_ptr_cast<int volatile>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int volatile>(anyPtr).get());

    EXPECT_NO_THROW(any_shared_ptr_cast<int>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int>(anyPtr).get());
    EXPECT_NO_THROW(any_shared_ptr_cast<int const>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int volatile>(anyPtr).get());

  }
  // Test volatile -> volatile & non-volatile cast
  // Note volatile -> non-volatile should throw bad_any_shared_ptr_cast 
  {
    auto ptr = make_shared<int volatile>(42);
    any_shared_ptr anyPtr{ ptr };

    EXPECT_THROW(any_shared_ptr_cast<int>(anyPtr), bad_any_shared_ptr_cast);
    EXPECT_NO_THROW(any_shared_ptr_cast<int volatile>(anyPtr));
    ASSERT_EQ(ptr.get(), any_shared_ptr_cast<int volatile>(anyPtr).get());
  }
}

TEST(any_shared_ptr, make_any_shared_ptr)
{
  // Test 0 arguments
  {
    auto any = make_any_shared_ptr<int>();

    ASSERT_TRUE(any.has_value());
    ASSERT_EQ(any.type(), typeid(shared_ptr<int>));
    auto ptr = any_shared_ptr_cast<int>(any);
    ASSERT_EQ(*ptr, 0);
  }
  // Test with 2 arguments
  {
    auto any = make_any_shared_ptr<pair<int, string>>(42,"test");

    ASSERT_TRUE(any.has_value());
    ASSERT_EQ(any.type(),typeid(shared_ptr<pair<int, string>>));
    auto ptr = any_shared_ptr_cast<pair<int, string>>(any);
    ASSERT_EQ(ptr->first, 42);
    ASSERT_EQ(ptr->second, "test");
  }
}