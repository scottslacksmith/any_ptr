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
  std::cout << "sizeof(any_shared_ptr)       = " << sizeof(xxx::any_shared_ptr) << '\n';
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

