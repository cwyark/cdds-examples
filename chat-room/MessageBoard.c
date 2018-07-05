#include "ddsc/dds.h"
#include "Chat.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[])
{
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t reader;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

}