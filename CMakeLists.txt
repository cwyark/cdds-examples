
cmake_minimum_required(VERSION 3.10)

if (NOT TARGET CycloneDDS::ddsc)
    # Find the CycloneDDS package. If it is not in a default location, try
    # finding it relative to the example where it most likely resides.
    find_package(CycloneDDS REQUIRED PATHS /opt/CycloneDDS)
endif()

# CmakeToolHelpers is used for liux env
include(CMakeToolsHelpers OPTIONAL)

add_subdirectory(HelloWorld)
add_subdirectory(ChatRoom)
#add_subdirectory(OwnerShip)
add_subdirectory(TopicKeys)
