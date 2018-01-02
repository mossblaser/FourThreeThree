/**
 * A minimal 433 MHz radio library designed specifically to handle:
 *
 * * Receiving codes transmitted by Kerui wireless PIRs
 * * Transmitting codes used by cheap remote-controlled socket adapters (coming
 *   soon)
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
 * and length.
 *
 * Length gives the number of significant bits in the code. Code is always a
 * single unsigned long and any bits which don't fit in its length are
 * unavailable.
 */
bool FourThreeThree_rx(unsigned long *code, unsigned int *length);

/**
 * Stop listening for 433 MHz transmissions
 */
void FourThreeThree_rx_end();

#endif
