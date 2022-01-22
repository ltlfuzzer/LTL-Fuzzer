set(This automata)
set(Sources
    automata.cc
    exception.cc
)
add_library(${This} STATIC ${Sources})
target_include_directories(${This} PUBLIC ${CMAKE_SOURCE_DIR}/include)

