#ifndef IN4073_MESSAGE_HELPER_H__
#define IN4073_MESSAGE_HELPER_H__

#include "message/message.h"

void echo_message(uint8_t * message, uint8_t message_len);
void send_message(message_t * message, uint8_t message_len);
void send_mode_update(message_t * send_buffer, uint8_t mode);
void send_calibration_data(message_t * send_buffer, int16_t phi, int16_t theta, 
	int16_t psi, int16_t sp, int16_t sq, int16_t sr, int16_t sax, int16_t say, 
	int16_t saz);
uint8_t handle_message(message_t * send_buffer, uint8_t * receive_buffer, 
	uint8_t buffer_len, bool * demo_done);
void send_terminate(message_t * send_buffer);


#endif