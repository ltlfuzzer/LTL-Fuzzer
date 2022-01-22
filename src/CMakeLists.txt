set(This fuzzer)
set(Entry ltl-fuzz)

set(Sources
    LTLFuzzer.cc
    PathStore.cc
    TargetsStore.cc
    RandomStrategy.cc
    utils.cc
    AutomataHandler.cc
)

add_library(${This} STATIC ${Sources})
add_executable(${Entry} main.cc)
target_link_libraries(${Entry} PUBLIC
    ${This}
    pthread
    automata
    spot
    bddx
    rt
	boost_system

)

add_subdirectory(automata)
add_subdirectory(instrumentation)
add_subdirectory(aflgo-extension)
