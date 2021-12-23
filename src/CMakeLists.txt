set(project_source_files
  fsds.hpp
  fsds.cpp
  fsdsOptions.hpp
  list.hpp
  list.inl
)

add_library(${primary_target_name} ${project_source_files})