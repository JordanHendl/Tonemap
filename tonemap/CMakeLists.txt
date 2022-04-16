find_package(ohm REQUIRED)
add_compile_options( -g )
add_executable(tonemap main.cpp image.cpp)
target_link_libraries(tonemap ohm::vulkan ohm::io ${Stb_Tools_LIBRARIES})
target_include_directories(tonemap PUBLIC ${Stb_Tools_INCLUDE_DIRS})

set( SHADERS
    "shaders/intensify.comp.glsl"
    "shaders/reset_histogram.comp.glsl"
    "shaders/histogram.comp.glsl"
    "shaders/inclusive_scan.comp.glsl"
    "shaders/normalize_histogram.comp.glsl"
    "shaders/generate_tonemap.comp.glsl"
    "shaders/apply_tonemap.comp.glsl"
)

foreach( SHADER ${SHADERS} )
  file(COPY ${SHADER} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()


