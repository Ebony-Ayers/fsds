set(project_source_files
  fsds.hpp
  fsds.cpp
  fsds_options.hpp

  list.hpp
  list.inl
  
  linked_list.hpp
  linked_list.inl
  
  spsc_queue.hpp
  spsc_queue.inl
  
  queue_block.hpp
  spmc_queue.hpp
  spmc_queue.inl

  #static_string.hpp
  static_string.cpp
  #static_string.inl

  #dynamic_string.hpp
  dynamic_string.cpp
  #dynamic_string.inl

  #composite_string.hpp
  #composite_string.inl
)

add_library(${primary_target_name} ${project_source_files})