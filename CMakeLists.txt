cmake_minimum_required(VERSION 3.10)
project(ScientificCalculator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets package with all necessary components
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Add executable
add_executable(scientific_calculator main.cpp)

# Link with wxWidgets libraries
target_link_libraries(scientific_calculator ${wxWidgets_LIBRARIES})

# Add math library for advanced mathematical functions
target_link_libraries(scientific_calculator m)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(scientific_calculator PRIVATE -Wall -Wextra)
elseif(MSVC)
  target_compile_options(scientific_calculator PRIVATE /W4)
endif()

# Install target
install(TARGETS scientific_calculator DESTINATION bin)

# Package configuration
set(CPACK_PACKAGE_NAME "ScientificCalculator")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Euclidae")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Scientific Calculator using wxWidgets")
include(CPack)
