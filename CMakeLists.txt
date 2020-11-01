cmake_minimum_required (VERSION 3.10)
project (Genetic)

# The main executable
add_executable(Genetic
  main.cpp
)

set_property(TARGET Genetic PROPERTY CXX_STANDARD 11)
target_compile_options(Genetic PRIVATE -Wall)

