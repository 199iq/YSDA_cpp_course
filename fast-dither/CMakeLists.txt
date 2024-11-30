add_catch(test_fast_dither tests/test.cpp)
target_link_libraries(test_fast_dither PRIVATE ${PNG_LIBRARY})
target_link_libraries(test_fast_dither PRIVATE nanoflann::nanoflann)
