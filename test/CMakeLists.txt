project(tests LANGUAGES C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(curl_test curl_test.c)
target_link_libraries(curl_test httpdlopen)
add_test(NAME curl_test COMMAND curl_test)

add_executable(full_test full_test.c)
target_link_libraries(curl_test httpdlopen)
add_test(NAME full_test COMMAND full_test)
