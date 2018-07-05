idlc_generate(Chat_lib "Chat.idl")

add_executable(Chatter Chatter.c)
add_executable(MessageBoard MessageBoard.c)
add_executable(UserLoad UserLoad.c)

target_link_libraries(Chatter Chat_lib CycloneDDS::ddsc)
target_link_libraries(MessageBoard Chat_lib CycloneDDS::ddsc)
target_link_libraries(UserLoad Chat_lib CycloneDDS::ddsc)