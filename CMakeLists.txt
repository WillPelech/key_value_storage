cmake_minimum_required(VERSION 3.15)
project(key_value_storage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If yaml-cpp is installed via Homebrew on macOS, add these hints
if(APPLE)
    # Common Homebrew paths for Apple Silicon and Intel Macs
    set(yaml-cpp_DIR "/opt/homebrew/opt/yaml-cpp/lib/cmake/yaml-cpp")
    list(APPEND CMAKE_PREFIX_PATH 
        "/opt/homebrew"
        "/usr/local"
    )
endif()

# Find yaml-cpp package
find_package(yaml-cpp REQUIRED)

# Add executable
add_executable(kv kv.cpp)

# Link yaml-cpp (this also includes the headers automatically)
target_link_libraries(kv PRIVATE yaml-cpp::yaml-cpp)
