set(CMAKE_CXX_STANDARD 11)

add_executable(test_taumng_index test_taumng_index.cpp)
target_link_libraries(test_taumng_index ${PROJECT_NAME} -ltcmalloc)

add_executable(test_taumng_search test_taumng_search.cpp)
target_link_libraries(test_taumng_search ${PROJECT_NAME})
