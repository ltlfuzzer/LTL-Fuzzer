set(This aflgo-ext)
set(Sources
    aflgo_ext.c
)
add_library(${This} STATIC ${Sources})
target_include_directories(${This} PUBLIC ${CMAKE_SOURCE_DIR}/include)
