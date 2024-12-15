if (APPLE)
  link_directories(/usr/local/lib)
  link_directories(/usr/local/opt/openssl/lib)

  include_directories(/usr/local/include)
  include_directories(/usr/local/opt/openssl/include)
endif()

file(GLOB_RECURSE SOLUTION_SRC CONFIGURE_DEPENDS "telegram/*.cpp")
add_library(telegram ${SOLUTION_SRC})
target_link_libraries(telegram PUBLIC PocoNet PocoNetSSL PocoFoundation PocoJSON PocoUtil)
target_include_directories(telegram PUBLIC .)

add_shad_executable(bot-run telegram/main.cpp)
target_link_libraries(bot-run PRIVATE telegram)

add_subdirectory(fake)

add_catch(test_telegram tests/test_api.cpp tests/test_scenarios.cpp)
target_link_libraries(test_telegram PRIVATE telegram fake_telegram_server)
