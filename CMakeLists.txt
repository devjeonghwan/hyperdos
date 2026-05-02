cmake_minimum_required(VERSION 3.20)

project(hyperdos LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(hyperdos STATIC
        src/chipset.c
        src/devices.c
        src/hardware.c
        src/x86_16_processor.c
        src/pc_bios.c
        src/pc_bios_data_area.c
        src/pc_bios_runtime.c
        src/pc_board.c
        src/pc_cmos.c
        src/pc_disk_bios.c
        src/pc_disk_image.c
        src/pc_floppy_controller.c
        src/pc_firmware.c
        src/pc_keyboard_bios.c
        src/pc_machine.c
        src/pc_model.c
        src/pc_storage.c
        src/pc_system_bios.c
        src/pc_text.c
        src/pc_video_bios.c
        src/pc_video_services.c)
target_include_directories(hyperdos PUBLIC include)

if (MSVC)
    target_compile_options(hyperdos PRIVATE /W4)
else ()
    target_compile_options(hyperdos PRIVATE -Wall -Wextra -Wpedantic)
endif ()

if (WIN32)
    add_executable(hyperdos_win32_pc_monitor WIN32
            platforms/win32/pc_directory_disk.c
            platforms/win32/pc_file_disk.c
            platforms/win32/pc_monitor_app.c
            platforms/win32/pc_monitor.c
            platforms/win32/pc_monitor_main.c
            platforms/win32/pc_monitor_runtime.c)
    target_link_libraries(hyperdos_win32_pc_monitor PRIVATE hyperdos user32 gdi32 comdlg32 shell32 ole32 comctl32)
endif ()

add_executable(hyperdos_tests tests/emulator_tests.c)
target_link_libraries(hyperdos_tests PRIVATE hyperdos)

enable_testing()
add_test(NAME hyperdos_tests COMMAND hyperdos_tests)
