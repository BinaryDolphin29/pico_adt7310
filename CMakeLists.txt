add_executable(adt7310 adt7310.c)
target_link_libraries(adt7310 pico_stdlib hardware_spi)

pico_enable_stdio_usb(adt7310 1)
pico_enable_stdio_uart(adt7310 0)

pico_add_extra_outputs(adt7310)