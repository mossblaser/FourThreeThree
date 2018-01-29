/**
 * A minimal 433 MHz radio library designed specifically to handle:
 *
 * * Receiving codes transmitted by Kerui wireless PIRs
 * * Transmitting codes used by cheap 'Mercury' remote-controlled socket
 *   adapters
 */

#ifndef FOURTHREETHREE_H
#define FOURTHREETHREE_H

/**
 * Start listening for 433 MHz transmissions.
 *
 * @param pin The pin the 433 MHz radio receiver is connected to.
 * @param zero_min_us Minimum positive pulse width for a 'zero' symbol.
 * @param zero_max_us Maximum positive pulse width for a 'zero' symbol.
 * @param one_min_us Minimum positive pulse width for a 'one' symbol.
 * @param one_max_us Maximum positive pulse width for a 'one' symbol.
 * @param symbol_max_us Maximum posedge-to-next-posedge duration for a valid symbol.
 */
void FourThreeThree_rx_begin(int pin,
                             unsigned long zero_min_us=200ul,
                             unsigned long zero_max_us=400ul,
                             unsigned long one_min_us=800ul,
                             unsigned long one_max_us=1100ul,
                             unsigned long symbol_max_us=1500ul);

/**
 * Get a received code. Returns true if a code has been received and sets code
 * and length. Call regularly.
 *
 * Length gives the number of significant bits in the code. Code is always a
 * single unsigned long and any bits which don't fit in its length are
 * unavailable.
 */
bool FourThreeThree_rx(unsigned long *code, unsigned int *length);


/**
 * Prepare a pin for use in transmitting 433 MHz signals.
 *
 * @param pin The pin the 433 MHz radio receiver is connected to.
 * @param zero_on_us Duration of positive pulse when sending a 'zero' symbol.
 * @param zero_off_us Duration of negative pulse when sending a 'zero' symbol.
 * @param one_on_us Duration of positive pulse when sending a 'one' symbol.
 * @param one_off_us Duration of negative pulse when sending a 'one' symbol.
 * @param inter_code_gap_ms Duration of pause between adjacent codes.
 * @param repeats Number of repetitions to send of a code.
 */
void FourThreeThree_tx_begin(int pin,
                             unsigned long zero_on_us = 170ul,
                             unsigned long zero_off_us = 600ul,
                             unsigned long one_on_us = 550ul,
                             unsigned long one_off_us = 220ul,
                             unsigned long inter_code_gap_ms = 6,
                             unsigned int repeats = 20);

/**
 * Transmit a code. Returns 'true' if transmission has started, 'false' if the
 * transmitter is stlil busy with another code.
 */
bool FourThreeThree_tx(unsigned long code, unsigned int length);

/**
 * Call frequently while using the TX functions.
 */
void FourThreeThree_tx_loop();

#endif
