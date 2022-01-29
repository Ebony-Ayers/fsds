set(project_source_files
  fsds.hpp
  fsds.cpp
  fsdsOptions.hpp
  list.hpp
  list.inl
  linked_list.hpp
  linked_list.inl
  spsc_queue.hpp
  spsc_queue.inl
)

add_library(${primary_target_name} ${project_source_files})