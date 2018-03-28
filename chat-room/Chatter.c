#include "ddsc/dds.h"
#include "Chat.h"
#include "stdio.h"
#include "stdlib.h"

#define CHAT_MSG_CONTENT_SIZE 36

int main(int argc, char *argv[]) {
  dds_entity_t participant;
  dds_entity_t ChatMessage_topic;
  dds_entity_t NameService_topic;
  dds_entity_t ChatMessage_writer;
  dds_entity_t NameService_writer;
  dds_qos_t *reliable_qos;
  dds_qos_t *settings_qos;
  dds_return_t ret;
  Chat_ChatMessage ChatMsg;
  Chat_NameService NameSrv;
  uint16_t ownID = 1;
  uint16_t send_count = 10;

  /* Create a participant from the default domain */
  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Create a topic with QoS policy `reliable:RELIABILITY` and register Chat:ChatMessage message type */
  reliable_qos = dds_qos_create();
  dds_qset_reliability(reliable_qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  ChatMessage_topic = dds_create_topic(participant, &Chat_ChatMessage_desc, \
    "Chat_ChatMessage", reliable_qos, NULL);
  dds_qos_delete(reliable_qos);
  DDS_ERR_CHECK(ChatMessage_topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Create a topic with QoS policy `durability:TRANSIENT` and register for Chat:NameService message type */
  settings_qos = dds_qos_create();
  dds_qset_durability(settings_qos, DDS_DURABILITY_TRANSIENT);
  NameService_topic = dds_create_topic(participant, &Chat_NameService_desc, \
    "Chat_NameService", settings_qos, NULL);
  dds_qos_delete(settings_qos);
  DDS_ERR_CHECK(NameService_topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Create a NameService data writer to inform there's a member joined the chat room */
  NameService_writer = dds_create_writer(participant, NameService_topic, NULL, NULL);
  DDS_ERR_CHECK(NameService_writer, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Publish a hello message to the whole chat room, actually it will subscribe in the UserLand.c*/
  NameSrv.userID = ownID;
  /* Note that NameSrv.name is pre-allocted buffer within compile time. So you should not
   * allocate a new memory and assign the pointer directly, ex: NameSrv.name = Chat_nameType__alloc();
   * */
  sprintf(NameSrv.name, "%s", "Chester");
  ret = dds_write(NameService_writer, &NameSrv);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Create a ChatMessage data writer, it will inherit QoS settings from the topic */
  printf("[Chatter] Saying hello to the chat room .. \n");
  ChatMessage_writer = dds_create_writer(participant, ChatMessage_topic, NULL, NULL);
  DDS_ERR_CHECK(ChatMessage_writer, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  printf("[Chatter] Waiting for a message board to be discovered ... \n");

  /* Enable the DDS_PUBLICATION_MATCHED_STATUS flag to ensure the message data reader
   * has been found and matched.
   * */
  ret = dds_set_enabled_status(ChatMessage_writer, DDS_PUBLICATION_MATCHED_STATUS);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Blocking current thread until match flag has been enabled or a timeout: 20 secs */
  uint32_t count_down = 20;

  while(count_down--) {
    uint32_t status ;
    ret = dds_get_status_changes(ChatMessage_writer, &status);
    DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    if (status == DDS_PUBLICATION_MATCHED_STATUS) {
      goto pub_msg;
    }
    printf("[Chatter] Waiting for a matched topic, count down: %d\n", count_down);
    dds_sleepfor(DDS_SECS(1));
  }
  printf("[Chatter] Could not find matched topic, return.");
  goto finalized;

pub_msg:
  /* Publish `send_count` times message */
  ChatMsg.content = dds_string_alloc(CHAT_MSG_CONTENT_SIZE);
  for(uint16_t i=0; i<send_count; i++) {
    ChatMsg.userID = ownID;
    ChatMsg.index = i;
    sprintf(ChatMsg.content, "[Chatter]Hello World Message count: %d from %d", ChatMsg.index, ChatMsg.userID);
    ret = dds_write(ChatMessage_writer, &ChatMsg);
    DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
  }
  dds_string_free(ChatMsg.content);

finalized:
  /* Finalized your code */
  ret = dds_delete(participant);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
  printf("End of program");
  return EXIT_SUCCESS;
}