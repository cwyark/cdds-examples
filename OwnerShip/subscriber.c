#include "OwnerShipData.h"
#include "ddsc/dds.h"
#include "stdio.h"
#include "stdlib.h"
#include "argparse.h"

#define MAX_SAMPLES 100
#define WAIT_EXIT_TIMEOUT 100

static const char *const usage[] = {
  "OwnerShipSubscriber [options] [[--] args]",
  NULL
};


int main(int argc, char** argv)
{
  dds_entity_t participant;
  dds_qos_t *tQos;
  dds_entity_t topic;
  dds_qos_t *rQos;
  dds_entity_t reader;
  dds_entity_t waitSet;
  dds_entity_t readCond;
  dds_return_t result_returned;
  dds_return_t samples_returned;
  bool ownership_exclusive = false;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_GROUP("Basic options"),
    OPT_BOOLEAN('e', "exclusive", &ownership_exclusive, "Ownership exclusive. Default: false"),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usage, 0);
  argc = argparse_parse(&argparse, argc, (const char**)argv);
  if (argc != 0) {
    argparse_usage(&argparse);
    exit(EXIT_SUCCESS);
  }

  /* Create a Participant. */
  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  tQos = dds_create_qos();
  dds_qset_reliability(tQos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  if (ownership_exclusive == true) {
    dds_qset_ownership(tQos, DDS_OWNERSHIP_EXCLUSIVE);
  } else {
    dds_qset_ownership(tQos, DDS_OWNERSHIP_SHARED);
  }

  /* Create a Topic. */
  topic = dds_create_topic(
      participant, &OwnerShipData_Stock_desc, "OwnerShipData_Stock", tQos, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a reliable Reader. */
  rQos = dds_create_qos();
  result_returned = dds_copy_qos(rQos, tQos);
  if (result_returned < 0)
    DDS_FATAL("dds_copy_qos: %s", dds_strretcode(-result_returned));
  dds_delete_qos(tQos);

  reader = dds_create_reader(participant, topic, rQos, NULL);
  if (reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
  dds_delete_qos(rQos);

  // Create a waitSet
  waitSet = dds_create_waitset(participant);
  if (waitSet < 0)
    DDS_FATAL("dds_create_waitset: %s\n", dds_strretcode(-waitSet));

  // Creater a read condition
  readCond = dds_create_readcondition(reader, DDS_ANY_STATE);
  if (readCond < 0)
    DDS_FATAL("dds_create_readcondition: %s\n", dds_strretcode(-readCond));

  // Attach read condition to waitSet
  result_returned = dds_waitset_attach(waitSet, readCond, reader);
  if (result_returned < 0)
    DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-result_returned));

  uint32_t status;
  uint32_t timeout = 5000;

  printf("Waiting for a matched publisher\n");

  do {
    result_returned = dds_get_status_changes(reader, &status);
    if (result_returned != DDS_RETCODE_OK)
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-result_returned));
    dds_sleepfor(DDS_MSECS(20));
    timeout --;
  } while (!(status & DDS_SUBSCRIPTION_MATCHED_STATUS) && (timeout != 0));

  if (timeout == 0) {
    printf("waiting for a matched publisher timeout, exit...\n");
    goto cleanup;
  }

  printf("\n=== [Subscriber] Waiting for samples, presse ctrl-C to exit ===\n");

  dds_attach_t triggered;

  // Wait for any status changed
  while (!dds_triggered(waitSet)) {
    result_returned = dds_waitset_wait(waitSet, &triggered, 1, DDS_SECS(WAIT_EXIT_TIMEOUT));
    if (result_returned == 0) {
      printf("There's no incoming data for %d seconds, timeout and exit\n",
          WAIT_EXIT_TIMEOUT);
      exit(EXIT_FAILURE);
    }
    for (uint8_t readers = 0; readers < result_returned; readers++) {
      void *samples[MAX_SAMPLES] = {0};
      dds_sample_info_t infos[MAX_SAMPLES];
      samples_returned = dds_take(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
      if (samples_returned < 0) {
        DDS_FATAL("dds_read: %s\n", samples_returned);
      } else {
        if (samples_returned == 0)
          continue;
        for (uint16_t j = 0; j < samples_returned; j++) {
          if (infos[j].valid_data) {
            OwnerShipData_Stock* msg = (OwnerShipData_Stock*)samples[j];
            printf("Sample: [ticker]: %s, [price]:%f, [publisher]:%s, strength:[%d]\n",
                msg->ticker, msg->price, msg->publisher, msg->strength);
          }
        }
      }
      dds_return_loan(reader, samples, samples_returned);
    }
  }

cleanup:
  /* Deleting the participant will delete all its children recursively as well. */
  result_returned = dds_delete(participant);
  if (result_returned != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-result_returned));

  return EXIT_SUCCESS;
}
