# CMake generated Testfile for 
# Source directory: /home/frederick/Project/ccm
# Build directory: /home/frederick/Project/ccm/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ccm_tests "/home/frederick/Project/ccm/build/ccm_tests")
set_tests_properties(ccm_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/frederick/Project/ccm/CMakeLists.txt;60;add_test;/home/frederick/Project/ccm/CMakeLists.txt;0;")
subdirs("third_party/FTXUI")
