file(GLOB_RECURSE _source_list CONFIGURE_DEPENDS "*.c" "*.cpp" "*.h" "*.hpp")

add_library(EmulatorCore STATIC ${_source_list})

set_target_properties(EmulatorCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(EmulatorCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "../" "" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
