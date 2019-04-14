#include "ddsc/dds.h"
#include "Chat.h"
#include "stdio.h"
#include "stdlib.h"


int main(int argc, char *argv[])
{
  dds_entity_t participant;
  dds_entity_t ChatMessage_topic;
  dds_entity_t NameService_topic;
  dds_entity_t NameService_reader;
  dds_qos_t *reliable_qos;
  dds_qos_t *setting_qos;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  /* DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT); */

  reliable_qos = dds_create_qos();
  dds_qset_reliability(reliable_qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  ChatMessage_topic = dds_create_topic(participant, &Chat_ChatMessage_desc, \
    "Chat_ChatMessage", reliable_qos, NULL);
  dds_delete_qos(reliable_qos);
  /* DDS_ERR_CHECK(ChatMessage_topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT); */

  setting_qos = dds_create_qos();
  dds_qset_durability(setting_qos, DDS_DURABILITY_TRANSIENT);
  NameService_topic = dds_create_topic(participant, &Chat_NameService_desc, \
    "Chat_NameService", setting_qos, NULL);
  dds_delete_qos(setting_qos);
  /* DDS_ERR_CHECK(NameService_topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT); */

  NameService_reader = dds_create_reader(participant, NameService_topic, NULL, NULL);

}
