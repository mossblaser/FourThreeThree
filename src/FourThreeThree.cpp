#include <Arduino.h>

#include "FourThreeThree.h"

static int rx_pin;

static unsigned long rx_zero_min_us;
static unsigned long rx_zero_max_us;
static unsigned long rx_one_min_us;
static unsigned long rx_one_max_us;
static unsigned long rx_symbol_max_us;

static volatile unsigned long rx_last_code = 0;
static volatile unsigned int rx_last_length = 0;

void on_rx_change() {
	// micros() on last posedge
	static unsigned long last_posedge = 0;
	
	// The code currently being received
	static unsigned long code = 0;
	static unsigned int code_length = 0;
	
	bool state = digitalRead(rx_pin);
	
	unsigned long now = micros();
	unsigned long duration = now - last_posedge;
	
	if (state) {  // Posedge
		// We've observed a pause between symbols, that means we've reached the end
		// of a received code.
		if (duration > rx_symbol_max_us) {
			// Discard extremely short messages since real messages tend to be
			// relatively lengthy (tens of bits) to ensure make them distinct from
			// noise.
			if (code_length >= 10) {
				rx_last_code = code;
				rx_last_length = code_length;
			}
			
			code = 0;
			code_length = 0;
		}
		
		last_posedge = now;
	} else {  // Negedge
		if (duration >= rx_zero_min_us && duration <= rx_zero_max_us) {
			// Zero
			code = (code << 1) | 0;
			code_length++;
		} else if (duration >= rx_one_min_us && duration <= rx_one_max_us) {
			// One
			code = (code << 1) | 1;
			code_length++;
		} else {
			// Bad pulse length, start again -- probably just noise.
			code = 0;
			code_length = 0;
		}
	}
}

void FourThreeThree_rx_begin(int pin,
                             unsigned long zero_min_us=200ul,
                             unsigned long zero_max_us=400ul,
                             unsigned long one_min_us=800ul,
                             unsigned long one_max_us=1100ul,
                             unsigned long symbol_max_us=1500ul) {
	rx_pin = pin;
	rx_zero_min_us = zero_min_us;
	rx_zero_max_us = zero_max_us;
	rx_one_min_us = one_min_us;
	rx_one_max_us = one_max_us;
	rx_symbol_max_us = symbol_max_us;
	
	// Set received code to 'empty'
	rx_last_code = 0;
	rx_last_length = 0;
	
	pinMode(rx_pin, INPUT);
	attachInterrupt(digitalPinToInterrupt(rx_pin), on_rx_change, CHANGE);
}

bool FourThreeThree_rx(unsigned long *code, unsigned int *length) {
	noInterrupts();
	
	bool valid = false;
	if (rx_last_length != 0) {
		valid = true;
		*code = rx_last_code;
		*length = rx_last_length;
		
		rx_last_code = 0;
		rx_last_length = 0;
	}
	
	interrupts();
	return valid;
}

void FourThreeThree_rx_end() {
	detachInterrupt(digitalPinToInterrupt(rx_pin));
}
