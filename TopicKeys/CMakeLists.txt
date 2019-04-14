idlc_generate(TopicKeys_lib "TopicKeys.idl")

# Both executables have only one related source file.
add_executable(TopicKeyedPublisher keyed/publisher.c)
add_executable(TopicKeyedSubscriber keyed/subscriber.c)

# Both executables need to be linked to the idl data type library and
# the ddsc API library.
target_link_libraries(TopicKeyedPublisher TopicKeys_lib CycloneDDS::ddsc)
target_link_libraries(TopicKeyedSubscriber TopicKeys_lib CycloneDDS::ddsc)

# Both executables have only one related source file.
add_executable(TopicKeylessPublisher keyless/publisher.c)
add_executable(TopicKeylessSubscriber keyless/subscriber.c)

# Both executables need to be linked to the idl data type library and
# the ddsc API library.
target_link_libraries(TopicKeylessPublisher TopicKeys_lib CycloneDDS::ddsc)
target_link_libraries(TopicKeylessSubscriber TopicKeys_lib CycloneDDS::ddsc)
