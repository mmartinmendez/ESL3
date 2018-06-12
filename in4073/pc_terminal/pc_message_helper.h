#include "../message/message.h"

void build_and_send_message (uint8_t msg_type, message_t * send_buffer);
void send_message(message_t * message, uint8_t message_len);
uint8_t select_message(uint8_t c, message_t * send_buffer);
uint8_t handle_message(message_t * buffer, uint8_t buffer_len);