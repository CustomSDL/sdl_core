set(NET_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOURCES
  ${NET_DIR}/src/*
)

if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
  list(REMOVE_ITEM SOURCES
    ${NET_DIR}/src/linux_tun_adapter.cc)
else ()
  list(REMOVE_ITEM SOURCES
    ${NET_DIR}/src/qnx_tun_adapter.cc)
endif ()

set(LIBRARIES
)

include_directories(
  ${NET_DIR}/..
  ${CMAKE_SOURCE_DIR}/src/components/utils
)

add_library("Net" ${SOURCES})

target_link_libraries("Net" ${LIBRARIES})
