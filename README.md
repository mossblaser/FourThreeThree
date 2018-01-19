FourThreeThree: Recieve command codes from simple 433 MHz radios
================================================================

A minimal Arduino library designed specifically to handle:

* Receiving codes transmitted by Kerui wireless PIRs
* Transmitting codes used by cheap remote-controlled socket adapters

This library is intended for my personal use and so is woefully incomplete,
inflexible and basic.

API synopsis ([full documentation in header](./src/FourThreeThree.h)):

API for receiving 433 MHz signals:

    #include "FourThreeThree.h"
    
    void FourThreeThree_rx_begin(int pin,
                                 unsigned long zero_min_us=200ul,
                                 unsigned long zero_max_us=400ul,
                                 unsigned long one_min_us=800ul,
                                 unsigned long one_max_us=1100ul,
                                 unsigned long symbol_max_us=1500ul);
    
    bool FourThreeThree_rx(unsigned long *code, unsigned int *length);
    
    void FourThreeThree_rx_end();

API for transmitting 433 MHz signals:

    void FourThreeThree_tx_begin(int pin,
                                 unsigned long zero_on_us = 170ul,
                                 unsigned long zero_off_us = 600ul,
                                 unsigned long one_on_us = 550ul,
                                 unsigned long one_off_us = 220ul,
                                 unsigned long inter_code_gap_ms = 6,
                                 unsigned int repeats = 20);
    
    bool FourThreeThree_tx(unsigned long code, unsigned int length);
    
    void FourThreeThree_tx_loop();

For use with Platform IO projects, add the following to an env section of your
`platformio.ini`:

    lib_debs = https://github.com/mossblaser/FourThreeThree.git

Just to reiterate, this library is heavily tweaked to my own bizarre
requirements and to rub this in notice that the default signal timings for TX
and RX are incompatible!! The RX timings are designed to pick up codes
sent by "Kerui" brand 433 MHz PIRs. The TX timings are designed to work with
"Mercury" brand remote controlled sockets.
