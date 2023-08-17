function(force_redefine_file_macro_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        # Get the relative path of the source file in project directory
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        string(REPLACE ${PROJECT_SOURCE_DIR}/ "" relpath ${filepath})
        list(APPEND defs "__FILE__=\"${relpath}\"")
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs}
            )
    endforeach()
endfunction()
# 遍历指定目标的源代码文件列表，为每个源文件添加一个宏定义，该宏定义会将 __FILE__ 替换为该源文件相对于项目目录的路径。
# 这样，在编译过程中，源文件中的 __FILE__ 宏会被替换为相应的路径信息。