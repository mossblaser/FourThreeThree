#include <Arduino.h>
#include "FourThreeThree.h"

static const int rx_pin = 2;

void setup() {
	Serial.begin(9600);
	
	FourThreeThree_rx_begin(rx_pin);
}


typedef struct {
	unsigned int length;
	unsigned long code;
	const char *name;
} code_t;

static const code_t expected_codes[] = {
	{25u, 0x0f64e14ul, "PIR Without Bracket"},
	{25u, 0x01c7614ul, "PIR With Bracket"},
	{0u, 0ul},
};

void loop() {
	unsigned long code;
	unsigned int code_length;
	
	if (FourThreeThree_rx(&code, &code_length)) {
		const code_t *expected_code = expected_codes;
		while (expected_code->length) {
			if (expected_code->length == code_length &&
			    expected_code->code == code) {
				Serial.println(expected_code->name);
				break;
			}
			expected_code++;
		}
	}
}

