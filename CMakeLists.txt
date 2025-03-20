cmake_minimum_required (VERSION 3.22)

project (Mem_driver
    DESCRIPTION "Memory driver"
    VERSION 1.0.0
    LANGUAGES C )

add_library(Mem_driver)

target_sources( Mem_driver PRIVATE
    Mem.c
)

target_include_directories( Mem_driver PUBLIC
    .
)

target_link_libraries( Mem_driver PRIVATE
    Platform_Types
    I2C_driver
)