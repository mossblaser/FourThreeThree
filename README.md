FourThreeThree: Recieve command codes from simple 433 MHz radios
================================================================

A minimal Arduino library designed specifically to handle:

* Receiving codes transmitted by Kerui wireless PIRs
* Transmitting codes used by cheap remote-controlled socket adapters (coming
  soon)

This library is intended for my personal use and so is woefully incomplete,
inflexible and basic.

API synopsis ([full documentation in header](./src/FourThreeThree.h)):

    #include "FourThreeThree.h"
    
    void FourThreeThree_rx_begin(int pin,
                                 unsigned long zero_min_us=200ul,
                                 unsigned long zero_max_us=400ul,
                                 unsigned long one_min_us=800ul,
                                 unsigned long one_max_us=1100ul,
                                 unsigned long symbol_max_us=1500ul);
    
    bool FourThreeThree_rx(unsigned long *code, unsigned int *length);
    
    void FourThreeThree_rx_end();

For use with Platform IO projects, add the following to an env section of your
`platformio.ini`:

    lib_debs = https://github.com/mossblaser/FourThreeThree.git


