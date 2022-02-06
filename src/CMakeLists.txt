find_package(glog)
find_package(GTest)

add_executable(test0 test.cc)
target_link_libraries(test0 PRIVATE glog::glog GTest::gtest
                                    GTest::gtest_main)
KyDepsPackageTarget(test0)
gtest_discover_tests(test0)

add_executable(test0_dd test_dd.cc)
target_link_libraries(test0_dd PRIVATE glog::glog GTest::gtest
                                    GTest::gtest_main)
KyDepsPackageTarget(test0_dd)
gtest_discover_tests(test0_dd)
