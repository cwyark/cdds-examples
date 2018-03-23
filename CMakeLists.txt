cmake_minimum_required(VERSION 3.5)
if (NOT TARGET CycloneDDS::ddsc)
    # Find the CycloneDDS package. If it is not in a default location, try
    # finding it relative to the example where it most likely resides.
    find_package(CycloneDDS REQUIRED PATHS ../../)
endif()

# CmakeToolHelpers is used for liux env
include(CMakeToolsHelpers OPTIONAL)

include_directories(/usr/local/include/)
add_subdirectory(hello-world)
add_subdirectory(chat-room)