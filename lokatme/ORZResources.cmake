# for supporting ORZ resources

option(USE_ORZ_RESOURCES  "Using ORZ resources" ON)

if (USE_ORZ_RESOURCES)
    add_definitions(-DUSE_ORZ_RESOURCES)
    message(STATUS "Using ORZ resources: ON")
else()
    message(STATUS "Using ORZ resources: OFF")
endif()

function(add_orz_resources var_output_dir var_headers var_sources)
    # checking orc file
    file(GLOB FOUND_ORZ_RESOURCES_ORC
            "${var_output_dir}/orz_resources.orc")

    list(LENGTH FOUND_ORZ_RESOURCES_ORC FOUND_ORZ_RESOURCES_ORC_SIZE)

    if (${FOUND_ORZ_RESOURCES_ORC_SIZE} LESS 1)
        message(STATUS "Generating empty orc file: ${var_output_dir}/orz_resources.orc")
        file(WRITE "${var_output_dir}/orz_resources.orc" "# Add every line to the format /url:path.\n")
    endif()

    set(${var_headers})
    set(${var_sources})

    list(APPEND ${var_headers} "${var_output_dir}/resources/orz_resources.h")
    list(APPEND ${var_sources} "${var_output_dir}/resources/orz_resources.c")

        message(STATUS "${var_output_dir}/resources")

    # compiling orc file
    add_custom_command(
            OUTPUT "${var_output_dir}/resources/orz_resources.c"
            COMMAND orz_resources "${var_output_dir}/orz_resources.orc" "--out_dir=${var_output_dir}/resources"
            DEPENDS "${var_output_dir}/orz_resources.orc"
            COMMENT "Compiling resources file ${var_output_dir}/orz_resources.orc"
            VERBATIM )

    set_source_files_properties(${${var_headers}} ${${var_sources}} PROPERTIES GENERATED TRUE)
    set(${var_headers} ${${var_headers}} PARENT_SCOPE)
    set(${var_sources} ${${var_sources}} PARENT_SCOPE)
endfunction()