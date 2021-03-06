#include "ddsc/dds.h"
#include "TopicKeys.h"
#include "stdio.h"
#include "stdlib.h"

int main (int argc, char **argv) {
  dds_return_t retcode;
  dds_return_t result_returned;
  dds_entity_t participant;
  dds_qos_t *tQos;
  dds_entity_t topic;
  dds_entity_t publisher;
  dds_qos_t *wQos;
  dds_entity_t writer;
  uint32_t iterations = 20;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  tQos = dds_create_qos();
  dds_qset_reliability(tQos, DDS_RELIABILITY_RELIABLE, DDS_SECS(2));

  topic = dds_create_topic(participant,
      &TopicKeys_KeylessMsg_desc, "TopicKeys_KeylessMsg",
      tQos, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  wQos = dds_create_qos();
  dds_copy_qos(wQos, tQos);
  writer = dds_create_writer(participant, topic, wQos, NULL);
  if (writer < 0)
    DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-writer));
  dds_delete_qos(wQos);
  dds_delete_qos(tQos);

  printf("=== [Publisher] Waiting for matching subscriber ===\n");

  result_returned = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
  if (result_returned != DDS_RETCODE_OK)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-result_returned));

  uint32_t status;

  // Wait for matching Data Reader
  do {
    result_returned = dds_get_status_changes(writer, &status);
    if (result_returned != DDS_RETCODE_OK)
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-result_returned));
    dds_sleepfor(DDS_MSECS(20));
  } while (!(status & DDS_PUBLICATION_MATCHED_STATUS));

  printf("=== [Publisher] Publishing keyless samples ===\n");

  for (uint32_t i = 0; i < iterations; i++) {
    for (uint32_t j = 0; j < iterations; j++) {
      TopicKeys_KeylessMsg msg;
      msg.userID = i;
      msg.value = j;
      printf("Sending msg: userID: %d, value: %d\n", msg.userID, msg.value);
      result_returned = dds_write(writer, &msg);
      if (result_returned != DDS_RETCODE_OK)
        DDS_FATAL("dds_writer: %s\n", dds_strretcode(-result_returned));
    }
  }

  result_returned = dds_delete(participant);
  if (result_returned != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-result_returned));

  exit(EXIT_SUCCESS);
}
