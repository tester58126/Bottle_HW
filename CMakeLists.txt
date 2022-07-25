
cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/pico-sdk)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(Bottle_HW_HID C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/main.c
    src/ssd1306.c
    src/usb_descriptors.c
    src/temperature.c
    src/usb_hid.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib 
    pico_multicore 
    hardware_adc
    hardware_spi 
    hardware_pio 
    hardware_dma
    hardware_irq
    hardware_vreg
    hardware_structs 
    tinyusb_device 
    tinyusb_board
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    src
)

target_link_options(${PROJECT_NAME} PRIVATE 
    -Xlinker
    --print-memory-usage
)

target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall
    -Wextra
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

pico_define_boot_stage2(slower_boot2 ${PICO_DEFAULT_BOOT_STAGE2_FILE})
target_compile_definitions(slower_boot2 PRIVATE PICO_FLASH_SPI_CLKDIV=4)
pico_set_boot_stage2(${PROJECT_NAME} slower_boot2)

pico_set_binary_type(${PROJECT_NAME} copy_to_ram)

pico_add_extra_outputs(${PROJECT_NAME})