idlc_generate(Chat_lib "Chat.idl")

add_executable(Chatter Chatter.c)
add_executable(MessageBoard MessageBoard.c)

target_link_libraries(Chatter Chat_lib CycloneDDS::ddsc)
target_link_libraries(MessageBoard Chat_lib CycloneDDS::ddsc)