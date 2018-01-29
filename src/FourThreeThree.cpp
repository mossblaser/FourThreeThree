#include <Arduino.h>

#include "FourThreeThree.h"

////////////////////////////////////////////////////////////////////////////////
// RX
////////////////////////////////////////////////////////////////////////////////

static int rx_pin;

static unsigned long rx_zero_min_us;
static unsigned long rx_zero_max_us;
static unsigned long rx_one_min_us;
static unsigned long rx_one_max_us;
static unsigned long rx_symbol_max_us;

static volatile unsigned long rx_last_code = 0;
static volatile unsigned int rx_last_length = 0;

static void on_rx_change() {
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
                             unsigned long zero_min_us,
                             unsigned long zero_max_us,
                             unsigned long one_min_us,
                             unsigned long one_max_us,
                             unsigned long symbol_max_us) {
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
}

bool FourThreeThree_rx(unsigned long *code, unsigned int *length) {
	// Watch for incoming signals
	static bool last_state;
	bool cur_state = digitalRead(rx_pin);
	if (last_state != cur_state) {
		on_rx_change();
		last_state = cur_state;
	}
	
	// Report if a signal has been received.
	if (rx_last_length != 0) {
		*code = rx_last_code;
		*length = rx_last_length;
		
		rx_last_code = 0;
		rx_last_length = 0;
		
		return true;
	} else {
		return false;
	}
}


////////////////////////////////////////////////////////////////////////////////
// TX
////////////////////////////////////////////////////////////////////////////////

static int tx_pin;
static unsigned long tx_zero_on_us;
static unsigned long tx_zero_off_us;
static unsigned long tx_one_on_us;
static unsigned long tx_one_off_us;
static unsigned long tx_inter_code_gap_ms;
static unsigned int tx_repeats;

// Code being transmitted
static unsigned int tx_code = 0;
static unsigned int tx_code_length = 0;

// Current bit being transmitted
static unsigned int tx_cur_bit_num = 0;

// How many repeats of the current code remain
static unsigned int tx_repeats_remaining = 0;

// Next transition time (microseconds)
unsigned long tx_next_transition_us;

// Next repeat start time (milliseconds)
unsigned long tx_next_repeat_ms;

void FourThreeThree_tx_begin(int pin,
                             unsigned long zero_on_us,
                             unsigned long zero_off_us,
                             unsigned long one_on_us,
                             unsigned long one_off_us,
                             unsigned long inter_code_gap_ms,
                             unsigned int repeats) {
	tx_pin = pin;
	tx_zero_on_us = zero_on_us;
	tx_zero_off_us = zero_off_us;
	tx_one_on_us = one_on_us;
	tx_one_off_us = one_off_us;
	tx_inter_code_gap_ms = inter_code_gap_ms;
	tx_repeats = repeats;
	
	pinMode(tx_pin, OUTPUT);
	digitalWrite(tx_pin, LOW);
	
	tx_code = 0;
	tx_code_length = 0;
	tx_cur_bit_num = 0;
	tx_repeats_remaining = 0;
}

static bool tx_cur_bit() {
	return (tx_code >> tx_cur_bit_num) & 1;
}

bool FourThreeThree_tx(unsigned long code, unsigned int length) {
	if (tx_repeats_remaining) {
		// TX still in progress, reject!
		return false;
	}
	
	tx_code = code;
	tx_code_length = length;
	
	// Start the transmission
	tx_cur_bit_num = tx_code_length; // One past the end means "we're mid repeat"
	tx_repeats_remaining = tx_repeats;
	tx_next_repeat_ms = millis(); // Now!
	
	return true;
}

void FourThreeThree_tx_loop() {
	if (!tx_repeats_remaining) {
		// Nothing to do!
		return;
	}
	
	bool cur_state = digitalRead(tx_pin);
	unsigned long now = micros();
	
	// NB: This timing code will bugger up when transitions occur across timer
	// wrapping boundaries... but who cares?
	if (cur_state) {
		// We're currently mid-bit!
		if (now >= tx_next_transition_us) {
			// Negative edge is due!
			digitalWrite(tx_pin, LOW);
			tx_next_transition_us += tx_cur_bit() ? tx_one_off_us : tx_zero_off_us;
		}
	} else {
		// We're coming towards the end of a bit/repeat delay
		
		// Deal with inter-repeat gaps
		if (tx_cur_bit_num == tx_code_length) {
			// We're currently in a repeat delay
			if (millis() < tx_next_repeat_ms) {
				// The delay is still counting down... do nothing for now.
				return;
			} else {
				// Delay over, transition now!
				tx_next_transition_us = now;
			}
		}
		
		// If we reach here we are potentially due to start the next bit!
		if (now >= tx_next_transition_us) {
			if (tx_cur_bit_num) {
				// Start the next bit
				tx_cur_bit_num--;
				digitalWrite(tx_pin, HIGH);
				tx_next_transition_us += tx_cur_bit() ? tx_one_on_us : tx_zero_on_us;
			} else {
				// We've just finished the last bit of the code, time to pause until
				// the next repeat
				tx_repeats_remaining--;
				
				tx_cur_bit_num = tx_code_length;
				tx_next_repeat_ms = millis() + tx_inter_code_gap_ms;
			}
		}
	}
}
