#include "TopicKeys.h"
#include "ddsc/dds.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_SAMPLES 10

int main(int argc, char** argv)
{
  dds_retcode_t retcode;
  dds_return_t samples_returned;
  dds_return_t result_returned;
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t subscriber;
  dds_qos_t* rQos;
  dds_entity_t reader;

  /* Create a participant */
  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create a topic */
  topic = dds_create_topic(participant,
      &TopicKeys_KeylessMsg_desc, "TopicKeys_KeylessMsg",
      NULL, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a subscriber */
  subscriber = dds_create_subscriber(participant, NULL, NULL);
  if (subscriber < 0)
    DDS_FATAL("dds_create_subscriber: %s\n", dds_strretcode(-subscriber));

  /* Create a reliable QoS data reader */
  rQos = dds_create_qos();
  dds_qset_reliability(rQos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  reader = dds_create_reader(participant, topic, rQos, NULL);
  if (reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
  dds_delete_qos(rQos);

  printf("\n=== [Subscriber] Waiting for a matched publisher ===\n");

  result_returned = dds_set_status_mask(reader, DDS_SUBSCRIPTION_MATCHED_STATUS);
  if (result_returned < 0)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-result_returned));

  uint32_t status;

  do {
    result_returned = dds_get_status_changes(reader, &status);
    if (result_returned != DDS_RETCODE_OK) {
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-result_returned));
    }
    dds_sleepfor(DDS_MSECS(10));
  } while (!(status & DDS_SUBSCRIPTION_MATCHED_STATUS));

  while (true) {
    void *samples[MAX_SAMPLES] = {0};
    dds_sample_info_t infos[MAX_SAMPLES];
    samples_returned = dds_take(reader, samples, infos, MAX_SAMPLES,
        MAX_SAMPLES);
    if (samples_returned < 0) {
      DDS_FATAL("dds_read: %s\n", samples_returned);
    } else {
      if (samples_returned == 0)
        continue;
      for (uint16_t j = 0; j < samples_returned; j++) {
        if (infos[j].valid_data) {
          TopicKeys_KeylessMsg* msg = (TopicKeys_KeylessMsg*)samples[j];
          printf("=== [Subscriber] Sample receoved ===\n");
          printf("Sample: [userID]: %d, [value]: %d\n", msg->userID, msg->value);
        } else {
          printf("invalid data: %d", j);
        }
      }
    }
    dds_return_loan(reader, samples, samples_returned);
    dds_sleepfor(DDS_MSECS(20));
  }

  result_returned = dds_delete(participant);
  if (result_returned != DDS_RETCODE_OK) {
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-result_returned));
  }

  exit(EXIT_SUCCESS);
}
