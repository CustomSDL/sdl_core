set(TRANSMITTER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOURCES
  ${TRANSMITTER_DIR}/src/*
)

set(LIBRARIES
    Utils
)

include_directories(
  ${TRANSMITTER_DIR}/..
  ${CMAKE_SOURCE_DIR}/src/components/utils
)

add_library("Transmitter" ${SOURCES})

target_link_libraries("Transmitter" ${LIBRARIES})
