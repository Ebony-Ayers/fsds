set(project_source_files
  string/static_string.cpp
  string/dynamic_string.cpp
  stable_list/stable_list.c
  slot_map/slot_map.c
  k_tree/k_tree.c
)

add_library(${primary_target_name} ${project_source_files})
set_source_files_properties(stable_list/stable_list.c slot_map/slot_map.c k_tree/k_tree.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)