#include "OwnerShipData.h"
#include "ddsc/dds.h"
#include "stdio.h"
#include "stdlib.h"
#include "argparse.h"

static const char *const usage[] = {
  "OwnerShipPublisher [options] [[--] args]",
  NULL
};

int main(int argc, char** argv)
{
  dds_entity_t participant;
  dds_qos_t *tQos;
  dds_entity_t topic;
  dds_qos_t* wQos;
  dds_entity_t writer;
  dds_return_t result_returned;
  const char* topic_name = NULL;
  const char* default_topic_name = "OwnerShipData_Stock";
  const char* publisher_name = NULL;
  int32_t strength = 0;
  uint32_t iteration = 10;
  bool ownership_exclusive = false;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_GROUP("Basic options"),
    OPT_STRING('p', "publisher", &publisher_name, "publisher name"),
    OPT_STRING('t', "topic", &topic_name, "selected topic"),
    OPT_INTEGER('s', "strength", &strength, "owner ship strength"),
    OPT_INTEGER('i', "iteration", &iteration, "iterations"),
    OPT_BOOLEAN('e', "exclusive", &ownership_exclusive, "Ownership exslusive. Default: false"),
    OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usage, 0);
  argc = argparse_parse(&argparse, argc, (const char **)argv);
  if (argc != 0) {
    argparse_usage(&argparse);
    exit(EXIT_SUCCESS);
  }

  if (topic_name == NULL) {
    topic_name = default_topic_name;
  }

  if (publisher_name == NULL)
    publisher_name = "publisher 1";

  /* Create a Participant. */
  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

  /* Create topic qos */
  tQos = dds_create_qos();
  dds_qset_reliability(tQos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
  if (ownership_exclusive == true) {
    dds_qset_ownership(tQos, DDS_OWNERSHIP_EXCLUSIVE);
  } else {
    dds_qset_ownership(tQos, DDS_OWNERSHIP_SHARED);
  }

  /* Create a Topic. */
  topic = dds_create_topic(
      participant, &OwnerShipData_Stock_desc, topic_name, tQos, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  wQos = dds_create_qos();
  result_returned = dds_copy_qos(wQos, tQos);
  if (result_returned < 0)
    DDS_FATAL("dds_copy_qos: %s\n", dds_strretcode(-result_returned));
  dds_qset_ownership_strength(wQos, strength);
  dds_delete_qos(tQos);

  /* Create a Writer. */
  writer = dds_create_writer(participant, topic, wQos, NULL);
  if (writer < 0)
    DDS_FATAL("dds_create_write: %s\n", dds_strretcode(-writer));
  dds_delete_qos(wQos);

  printf("=== [Publisher]  Waiting for a reader to be discovered ...\n");

  result_returned = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
  if (result_returned != DDS_RETCODE_OK)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-result_returned));

  uint32_t status;
  uint32_t timeout = 5000;

  do {
    result_returned = dds_get_status_changes(writer, &status);
    if (result_returned = DDS_RETCODE_OK)
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-result_returned));
    dds_sleepfor(DDS_MSECS(10));
    timeout --;
  } while (!(status & DDS_PUBLICATION_MATCHED_STATUS) && (timeout != 0));

  if (timeout == 0) {
    printf("Waiting for a matched subscriber timeout, exit\n");
    goto cleanup;
  }

  for (uint32_t i = 0; i < iteration; i++) {
    OwnerShipData_Stock msg;
    msg.ticker = dds_string_dup("MSFT");
    msg.price = i;
    msg.publisher = dds_string_dup(publisher_name);
    msg.strength = strength;
    printf("sending Message (%s %f %s %d)\n", msg.ticker, msg.price, msg.publisher, msg.strength);
    result_returned = dds_write(writer, &msg);
    if (result_returned != DDS_RETCODE_OK)
      DDS_FATAL("dds_write: %s\n", dds_strretcode(-result_returned));
    dds_sleepfor(DDS_USECS(10));
  }

cleanup:
  result_returned = dds_delete(participant);
  if (result_returned != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-result_returned));

  exit(EXIT_SUCCESS);
}
