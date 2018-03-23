#include "ddsc/dds.h"
#include "Chat.h"
#include "stdio.h"
#include "stdlib.h"

#define CHAT_MSG_CONTENT_SIZE 36

int main(int argc, char *argv[]) {
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t writer;
  dds_return_t ret;
  Chat_ChatMessage ChatMsg;
  uint16_t ownID = 1;
  uint16_t send_count = 10;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  topic = dds_create_topic(participant, &Chat_ChatMessage_desc, "Chat_ChatMessage", NULL, NULL);
  DDS_ERR_CHECK(topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  writer = dds_create_writer(participant, topic, NULL, NULL);
  DDS_ERR_CHECK(writer, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  ChatMsg.content = dds_string_alloc(CHAT_MSG_CONTENT_SIZE);

  for(uint16_t i=0; i<send_count; i++) {
    ChatMsg.userID = ownID;
    ChatMsg.index = i;
    sprintf(ChatMsg.content, "Hello World Message count: %d from %d", ChatMsg.index, ChatMsg.userID);
    ret = dds_write(writer, &ChatMsg);
    DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
  }
  dds_string_free(ChatMsg.content);
  
  ret = dds_delete(participant);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
  printf("End of program");
  return EXIT_SUCCESS;
}