set(This instrumentation)
set(Sources
    pathwriter.cc
    CodeBean.cc
    Instrument.cc
)

add_library(${This} STATIC ${Sources})
target_include_directories(${This} PUBLIC ${CMAKE_SOURCE_DIR}/include)

