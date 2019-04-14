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
  dds_entity_t topic;
  dds_entity_t writer;
  dds_return_t rc;
  OwnerShipData_Stock msg;
  uint32_t status = 0;
  const char* topic_name = NULL;
  const char* default_topic_name = "OwnerShipData_Stock";
  const char* publisher_name = NULL;
  int32_t strength = 0;
  uint32_t iteration = 10;
  dds_ownership_kind_t owner_kind;

  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_GROUP("Basic options"),
    OPT_STRING('p', "publisher", &publisher_name, "publisher name"),
    OPT_STRING('t', "topic", &topic_name, "selected topic"),
    OPT_INTEGER('s', "strength", &strength, "owner ship strength"),
    OPT_INTEGER('i', "iteration", &iteration, "iterations"),
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

  /* Create a Topic. */
  topic = dds_create_topic(
      participant, &OwnerShipData_Stock_desc, topic_name, NULL, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

  /* Create a Writer. */
  writer = dds_create_writer(participant, topic, NULL, NULL);
  if (writer < 0)
    DDS_FATAL("dds_create_write: %s\n", dds_strretcode(-writer));

  printf("=== [Publisher]  Waiting for a reader to be discovered ...\n");
  fflush(stdout);

  rc = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));

  while (!(status & DDS_PUBLICATION_MATCHED_STATUS)) {
    rc = dds_get_status_changes(writer, &status);
    if (rc != DDS_RETCODE_OK)
      DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-rc));

    /* Polling sleep. */
    dds_sleepfor(DDS_MSECS(20));
  }

  for (uint16_t i = 0; i < iteration; i++) {
    msg.ticker = dds_string_dup("MSFT");
    msg.price = 0.0;
    msg.publisher = dds_string_dup(publisher_name);
    msg.strength = strength;
    printf("sending Message (%s %f %s %d)\n", msg.ticker, msg.price, msg.publisher, msg.strength);
    rc = dds_write(writer, &msg);
    if (rc != DDS_RETCODE_OK)
      DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));
  }

  rc = dds_delete(participant);
  if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

  return EXIT_SUCCESS;
}
