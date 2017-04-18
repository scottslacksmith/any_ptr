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
#include <any_ptr.h>
#include <any_shared_ptr.h>
#include <crtdbg.h>
#include <iostream>

int main(int argc, char ** const argv)
{
  std::cout << "*** any_ptr ****" << '\n';
  std::cout << "sizeof(any_ptr)       = " << sizeof(xxx::any_ptr) << '\n';
  std::cout << "*** any_shared_ptr ****" << '\n';
  std::cout << "sizeof(std::shared_ptr<int>) = " << sizeof(std::shared_ptr<int>) << '\n';
  std::cout << "sizeof(ver_1::any_shared_ptr)       = " << sizeof(xxx::ver_1::any_shared_ptr) << '\n';
  std::cout << "sizeof(ver_2::any_shared_ptr)       = " << sizeof(xxx::ver_2::any_shared_ptr) << '\n';
  std::cout << std::endl;


#ifdef _MSC_VER
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
  // Redirect reported leaks to stderr 
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

