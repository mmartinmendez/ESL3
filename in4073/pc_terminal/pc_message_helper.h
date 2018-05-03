#include "../message/message.h"

void send_message(message_t * message, uint8_t message_len);
uint8_t select_message(uint8_t c, message_t * send_buffer);
void handle_message(message_t * buffer, uint8_t buffer_len);