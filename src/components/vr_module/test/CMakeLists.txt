include_directories (
  ${LOG4CXX_INCLUDE_DIRECTORY}
  ${GMOCK_INCLUDE_DIRECTORY}
  ${CMAKE_SOURCE_DIR}/src/components/functional_module/include/
  ${CMAKE_SOURCE_DIR}/src/components/vr_module/include/
  ${CMAKE_SOURCE_DIR}/src/components/include/
  ${CMAKE_SOURCE_DIR}/src/components/smart_objects/include
  ${CMAKE_SOURCE_DIR}/src/components/policy/src/policy/usage_statistics/include
  ${JSONCPP_INCLUDE_DIRECTORY}
  include
)

set (SOURCES
  src/vr_library_test.cc
  src/vr_proxy_test.cc
  src/support_service_test.cc
  src/activate_service_test.cc
  src/process_data_test.cc
  src/on_register_service_test.cc
  src/on_service_deactivated_test.cc
  src/on_default_service_chosen_test.cc
  src/on_unregister_service_test.cc
  src/socket_channel_test.cc
  src/integration_test.cc
)

set (LIBRARIES
  gtest
  gmock
  gmock_main
  vrmoduleplugin
  gcov
  pthread
)

if (ENABLE_TEST_COV_COUNT)
  set(GCOV_FLAGS "-ftest-coverage -fprofile-arcs")
else()
  set(GCOV_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCOV_FLAGS}")

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

SET(RPATH_DIRECTORIES
  ${CMAKE_BINARY_DIR}/src/components/vr_module/
  /usr/local/lib
  /usr/local
  ${CMAKE_BINARY_DIR}/src/components/utils
  ${GPB_LIBS_DIRECTORY}
)

SET(CMAKE_INSTALL_RPATH "${RPATH_DIRECTORIES}")

create_test("vr_module_test" "${SOURCES}" "${LIBRARIES}")
