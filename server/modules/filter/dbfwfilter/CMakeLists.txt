find_package(FLEX)
find_package(BISON)
if(BISON_FOUND AND FLEX_FOUND)
  bison_target(ruleparser ruleparser.y ${CMAKE_CURRENT_BINARY_DIR}/ruleparser.c COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/ruleparser.yy.h")
  flex_target(token token.l ${CMAKE_CURRENT_BINARY_DIR}/token.c COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/lex.yy.h")
  add_flex_bison_dependency(token ruleparser)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_library(dbfwfilter-core STATIC ${BISON_ruleparser_OUTPUTS} ${FLEX_token_OUTPUTS} rules.cc user.cc)

  # Depends on PCRE2
  add_dependencies(dbfwfilter-core pcre2 connector-c)

  add_library(dbfwfilter SHARED dbfwfilter.cc)
  target_link_libraries(dbfwfilter maxscale-common mysqlcommon dbfwfilter-core)
  set_target_properties(dbfwfilter PROPERTIES VERSION "1.0.0" LINK_FLAGS -Wl,-z,defs)
  install_module(dbfwfilter core)

  # The offline rule check utility
  add_executable(dbfwchk dbfw_rule_check.cc)
  target_link_libraries(dbfwchk maxscale-common mysqlcommon dbfwfilter-core)
  install_executable(dbfwchk core)

  if(BUILD_TESTS)
    add_subdirectory(test)
  endif()
elseif (BUILD_SYSTEM_TESTS)
  message(STATUS "Not building dbfwfilter when tests are built and both flex and bison were not found")
else()
    message(FATAL_ERROR "Could not find Bison or Flex: ${BISON_EXECUTABLE} ${FLEX_EXECUTABLE}")
endif()

