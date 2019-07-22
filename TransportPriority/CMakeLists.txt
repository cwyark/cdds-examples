idlc_generate(OwnerShipData_lib "OwnerShipData.idl")

# Both executables have only one related source file.
add_executable(OwnerShipPublisher publisher.c)
add_executable(OwnerShipSubscriber subscriber.c)

# Both executables need to be linked to the idl data type library and
# the ddsc API library.
target_link_libraries(OwnerShipPublisher OwnerShipData_lib CycloneDDS::ddsc argparse)
target_link_libraries(OwnerShipSubscriber OwnerShipData_lib CycloneDDS::ddsc argparse)
