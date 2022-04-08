set(project_source_files
  fsds.hpp
  fsds.cpp
  fsds_options.hpp

  list.hpp
  list.inl
  
  linked_list.hpp
  linked_list.inl
  
  #spmc_queue.cpp

  static_string.cpp
  dynamic_string.cpp

  #composite_string.hpp
  #composite_string.inl
)

add_library(${primary_target_name} ${project_source_files})