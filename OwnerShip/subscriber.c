#include "OwnerShipData.h"
#include "ddsc/dds.h"
#include "stdio.h"
#include "stdlib.h"

#define MAX_SAMPLES 100

int main(int argc, char** argv)
{
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t reader;
  dds_entity_t waitSet;
  dds_entity_t readCond;
  dds_duration_t waitTimeout = DDS_SECS(10);
  dds_attach_t wsresult[1];
  size_t wsresult_size = 1;
  OwnerShipData_Stock* msg;
  void* samples[MAX_SAMPLES];
  dds_sample_info_t infos[MAX_SAMPLES];
  dds_return_t rc;
  dds_qos_t* qos;
  (void)argc;
  (void)argv;

  /* Create a Participant. */
  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create a Topic. */
  topic = dds_create_topic(
      participant, &OwnerShipData_Stock_desc, "OwnerShipData_Stock", NULL, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a reliable Reader. */
  qos = dds_create_qos();
  dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  reader = dds_create_reader(participant, topic, qos, NULL);
  if (reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
  dds_delete_qos(qos);

  // Create a waitSet
  waitSet = dds_create_waitset(participant);

  // Creater a read condition
  readCond = dds_create_readcondition(reader, DDS_ANY_STATE);

  // Attach read condition to waitSet
  rc = dds_waitset_attach(waitSet, readCond, reader);
  if (rc < 0)
    DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-rc));

  printf("\n=== [Subscriber] Waiting for a sample ===\n");

  int sample_count = 0;

  // Wait for any status changed
  while (!dds_triggered(waitSet)) {
    rc = dds_waitset_wait(waitSet, wsresult, wsresult_size, waitTimeout);
    if (rc < 0) {
      DDS_FATAL("dds_waitset_wait: %s\n", dds_strretcode(-rc));
    }
    sample_count = dds_take(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
    printf(" sample_count : %d", sample_count);
  }

  /* Free the data location. */
  OwnerShipData_Stock_free(samples[0], DDS_FREE_ALL);

  /* Deleting the participant will delete all its children recursively as well. */
  rc = dds_delete(participant);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  return EXIT_SUCCESS;
}
