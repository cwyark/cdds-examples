#include "ddsc/dds.h"
#include "HelloWorldData.h"
#include "stdio.h"
#include "stdlib.h"

int main (int argc, char **argv) {
  dds_entity_t participant;
  dds_entity_t topic;
  dds_entity_t writer;
  dds_return_t ret;
  HelloWorldData_Msg msg;

  participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  DDS_ERR_CHECK(participant, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  topic = dds_create_topic(participant, &HelloWorldData_Msg_desc, "HelloWorldData_Msg", NULL, NULL);
  DDS_ERR_CHECK(topic, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  writer = dds_create_writer(participant, topic, NULL, NULL);

  printf("[Publisher] Waiting for a reader to be discovered ... \n");

  ret = dds_set_enabled_status(writer, DDS_PUBLICATION_MATCHED_STATUS);

  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  while(1) {
    uint32_t status;
    ret = dds_get_status_changes(writer, &status);
    DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    if(status == DDS_PUBLICATION_MATCHED_STATUS) {
      break;
    }
    dds_sleepfor(DDS_MSECS(20));
  }

  msg.userID = 1;

  /* Not that msg.message is pointer which point to a static string located in .bss section, 
   * the pointed memory can not be modified.
   * */

  msg.message = "HelloWorld";

  printf("[Publisher] Writing : ");
  printf("Message (%d, %s)\n", msg.userID, msg.message);

  ret = dds_write(writer, &msg);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  ret = dds_delete(participant);
  DDS_ERR_CHECK(ret, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

  return EXIT_SUCCESS;

}
