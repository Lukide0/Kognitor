cmake_minimum_required(VERSION 3.23)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain/avr.cmake")

project(
  kognitor
  VERSION 0.1.0
  LANGUAGES CXX ASM C)

include(cmake/bootstrap.cmake)


# ------------------------------------------------------------------------------
# Set options
option(LTO_OPTIMAZITION "Enable LTO optimization" ON)
set(MCU atmega328p)
set(FCPU 16000000)

set(MICROSTD_BUILD_EXAMPLES OFF)

# ------------------------------------------------------------------------------
# Target

include(cmake/microstd.cmake)
include(cmake/oled_display.cmake)

add_avr_executable(${PROJECT_NAME} ${MCU} ${FCPU})
target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    $<$<BOOL:${LTO_OPTIMAZITION}>:INTERPROCEDURAL_OPTIMIZATION TRUE>
)

target_link_libraries(${PROJECT_NAME} PRIVATE microstd)
target_link_libraries(${PROJECT_NAME} PRIVATE oled_display)

# ------------------------------------------------------------------------------
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

