set(target VRCooperation)
set(library_name ${CMAKE_SHARED_LIBRARY_PREFIX}${target}${CMAKE_SHARED_LIBRARY_SUFFIX})
set(install_destination ${CMAKE_BINARY_DIR}/bin/plugins)
set(copy_destination ${CMAKE_BINARY_DIR}/src/appMain/plugins)

if (ENABLE_GCOV)
  set(GCOV_FLAGS "-ftest-coverage -fprofile-arcs")
else()
  set(GCOV_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++0x -Wall -Werror ${GCOV_FLAGS}")

if (CMAKE_BUILD_TYPE)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "")
  else ()
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb3 -DDEBUG")
  endif()
endif()

include_directories (
  ./include/
  ../include/
  ../functional_module/include/
  ../application_manager/include
  ../connection_handler/include/
  ${JSONCPP_INCLUDE_DIRECTORY}
  ${LOG4CXX_INCLUDE_DIRECTORY}
  ${CMAKE_BINARY_DIR}/src/components/
  ${GPB_INCLUDE_DIRECTORY}
)

set (SOURCES
    ./src/vr_module.cc
    ./src/request_controller.cc
    ./src/vr_proxy.cc
    ./src/gpb_data_sender_receiver.cc
    ./src/mobile_command_factory.cc
    ./src/base_command_request.cc
    ./src/register_service_request.cc
    ./src/unregister_service_request.cc
    ./src/activate_service_request.cc
    ./src/vr_module_event.cc
    ./src/message_helper.cc
    ./src/on_service_deactivated_notification.cc
)

set (LIBRARIES
    jsoncpp
    FunctionalModule
    ApplicationManager
    Utils
    ConfigProfile
    protobuf
    HMIProto
)

generate_proto(${CMAKE_CURRENT_SOURCE_DIR}/interface/hmi.proto HMIProto)

link_directories(${LINK_DIRECTORIES} ${GPB_LIBS_DIRECTORY})
add_library(${target} SHARED ${SOURCES})
add_dependencies(${target} LibGPB)
target_link_libraries(${target} ${LIBRARIES} )

if(ENABLE_LOG)
  target_link_libraries("VRCooperation" log4cxx -L${LOG4CXX_LIBS_DIRECTORY})
endif()


install(TARGETS ${target}
  DESTINATION ${install_destination}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)
