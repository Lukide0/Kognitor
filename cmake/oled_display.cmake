include_guard(GLOBAL)

set(OLED_DISPLAY_SRC "${PROJECT_SOURCE_DIR}/vendor/oled_display")

add_library(oled_display)

set(OLED_DISPLAY_SOURCES
    "${OLED_DISPLAY_SRC}/lcd.h"
    "${OLED_DISPLAY_SRC}/lcd.c"

    "${OLED_DISPLAY_SRC}/font.c"
    "${OLED_DISPLAY_SRC}/font.h"

    "${OLED_DISPLAY_SRC}/i2c.c"
    "${OLED_DISPLAY_SRC}/i2c.h"
)

target_sources(oled_display PUBLIC
    ${OLED_DISPLAY_SOURCES}
)

target_include_directories(oled_display PUBLIC
    "${OLED_DISPLAY_SRC}"
)

set_source_files_properties(${OLED_DISPLAY_SOURCES} PROPERTIES LANGUAGE C)

target_compile_definitions(oled_display PRIVATE F_CPU=${FCPU})
target_compile_options(oled_display PRIVATE
    -mmcu=${MCU}
    -ffunction-sections
    -fdata-sections
    -Os
)

