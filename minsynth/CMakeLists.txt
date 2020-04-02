set(src
    synth.cc
    synth.h
)
source_group("" FILES ${src})

add_library(minsynth STATIC ${src})
target_link_libraries(minsynth
    PUBLIC core
    PRIVATE project_options project_warnings
)
