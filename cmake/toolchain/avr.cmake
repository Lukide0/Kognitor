find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE_TOOL avr-size REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)
find_program(AVR_STRIP avr-strip REQUIRED)

# ------------------------------------------------------------------------------
# Target

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_C_COMPILER "${AVR_CC}")
set(CMAKE_CXX_COMPILER "${AVR_CXX}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# ------------------------------------------------------------------------------
macro(add_avr_executable target_name avr_mcu f_cpu baud)
    set(elf_file ${target_name}-${avr_mcu}.elf)
    set(map_file ${target_name}-${avr_mcu}.map)
    set(hex_file ${target_name}-${avr_mcu}.hex)
    set(lst_file ${target_name}-${avr_mcu}.lst)
    set(strip_elf_file ${target_name}-${avr_mcu}.stripped.elf)

    add_executable(${target_name} ${ARGN})

    target_compile_definitions(${target_name} PRIVATE F_CPU=${f_cpu} BAUD=${baud})

    target_compile_options(${target_name} PRIVATE
        -mmcu=${avr_mcu}
        -ffunction-sections
        -fdata-sections
        -fno-exceptions
        -fno-rtti
        -nostdlib
        -nolibc
        -fno-threadsafe-statics
        -fshort-enums

        -Os

        -Wall
        -Wextra
        -Wformat
        -Wpedantic
        -Wshadow
        -Wunused
        -Wcast-qual
        -Wcast-align
        -Wdouble-promotion
        -Wimplicit-fallthrough
        -Wundef
        -Wfloat-equal
        -Wnull-dereference
        -Wextra-semi
        -Wold-style-cast
        -Wpessimizing-move
        -Wredundant-move
        -Wself-move
    )

    target_link_options(${target_name} PRIVATE
        -mmcu=${avr_mcu}
        -Wl,--gc-sections,--relax
        -Wl,-Map,${map_file}
    )

    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "${elf_file}")

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${AVR_OBJDUMP} -d ${elf_file} > ${lst_file}
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${AVR_OBJCOPY} -j .text -j .data -O ihex ${elf_file} ${hex_file}
        COMMAND ${AVR_SIZE_TOOL} ${elf_file}
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${AVR_STRIP} -o ${strip_elf_file} ${elf_file}
        COMMAND ${AVR_SIZE_TOOL} ${strip_elf_file}
    )


endmacro()

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
