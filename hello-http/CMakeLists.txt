function(link_poco NAME)
  target_link_libraries(${NAME} PRIVATE PocoNet PocoNetSSL PocoFoundation PocoJSON PocoUtil)
endfunction()

add_catch(test_hello_http test.cpp fake/fake_data.cpp weather.cpp)
link_poco(test_hello_http)

add_shad_executable(hello_http weather.cpp main.cpp)
link_poco(hello_http)
