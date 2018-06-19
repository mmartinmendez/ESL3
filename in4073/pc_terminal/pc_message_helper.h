#ifndef _PC_MESSAGE_HELPER_H_
#define _PC_MESSAGE_HELPER_H_

#include "../message/message.h"

void build_and_send_message (uint8_t msg_type, message_t * send_buffer); // Author: B.T. Blokland
void send_message(message_t * message, uint8_t message_len); // Author: B.T. Blokland
uint8_t select_message(uint8_t c, message_t * send_buffer); // Author: B.T. Blokland
uint8_t handle_message(message_t * buffer, uint8_t buffer_len); // Author: B.T. Blokland

#endif
