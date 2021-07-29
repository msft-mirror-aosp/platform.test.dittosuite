enable_testing()
find_package(GTest REQUIRED)

function(add_gtest test_src)
        get_filename_component(test_name ${test_src} NAME_WE)

        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} libdittosuite GTest::gtest GTest::gtest_main)

        gtest_discover_tests(${test_name})
endfunction()

file(GLOB ditto_test_srcs
        *.cpp
)

foreach(test_src IN ITEMS ${ditto_test_srcs})
        add_gtest(${test_src})
endforeach()
