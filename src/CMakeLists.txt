set(project_source_files
  static_string.cpp
  dynamic_string.cpp
  stable_list.c
  k_tree.c
)

add_library(${primary_target_name} ${project_source_files})
set_source_files_properties(stable_list.c k_tree.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)