idlc_generate(HelloWorldData_lib "HelloWorldData.idl")

# Both executables have only one related source file.
add_executable(HelloWorldPublisher publisher.c)
add_executable(HelloWorldSubscriber subscriber.c)

# Both executables need to be linked to the idl data type library and
# the ddsc API library.
target_link_libraries(HelloWorldPublisher HelloWorldData_lib CycloneDDS::ddsc)
target_link_libraries(HelloWorldSubscriber HelloWorldData_lib CycloneDDS::ddsc)
