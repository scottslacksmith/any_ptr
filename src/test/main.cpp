#include <crtdbg.h> 

int main(int argc, char ** const argv)
{
#ifdef _MSC_VER
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
  // Redirect reported leaks to stderr 
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

