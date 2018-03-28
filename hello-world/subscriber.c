#include "ddsc/dds.h"
#include "HelloWorldData.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_SAMPLES 1

int main(int argc, char **argv)
{
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t reader;
  HelloWorldData_Msg *msg;
  void *samples[MAX_SAMPLES];
  dds_sample_info_t infos[MAX_SAMPLES];
  dds_return_t ret;
  dds_qos_t *qos;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  topic = dds_create_topic(participant, &HelloWorldData_Msg_desc, "HelloWorldData_Msg", NULL, NULL);
  DDS_ERR_CHECK(topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  /* Create a reliable reader. */
  qos = dds_qos_create();
  dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  reader = dds_create_reader(participant, topic, qos, NULL);
  DDS_ERR_CHECK(reader, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
  dds_qos_delete(qos);

  printf("[Subscriber] Waiting for a sample ... \n");

  /** Initialize smaple buffer, by pointing the void pointer within
   * the buffer array to a valid sample memory localtion
   * */
  samples[0] = HelloWorldData_Msg__alloc();

  while(1) 
  {
    ret = dds_read(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
    DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    if ((ret > 0) && (infos[0].valid_data)) {
      msg = (HelloWorldData_Msg*) samples[0];
      printf("[Subscriber] Received  : ");
      printf("Mssage (%d, %s)\n", msg->userID, msg->message);
      break;
    } else {
      dds_sleepfor(DDS_MSECS(20));
    }
  }

  HelloWorldData_Msg_free(samples[0], DDS_FREE_ALL);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
}