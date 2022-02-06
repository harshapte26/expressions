find_package(glog)
find_package(GTest)

add_executable(test0 test.cc)
target_link_libraries(test0 PRIVATE glog::glog GTest::gtest
                                    GTest::gtest_main)
KyDepsPackageTarget(test0)
gtest_discover_tests(test0)
