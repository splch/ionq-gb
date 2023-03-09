#include <gbdk/platform.h>
#include <gb/gb.h>

#include "ionq_logo.h"

// Declare functions used later in the program
void init();
void graphics();
void audio();
void controls();

void main() {
    // Step 1: Initialize the Game Boy
    init();
    // Step 2: Display graphics
    graphics();
    // Step 3: Play audio
    audio();
    // Step 4: Check for user input
    while (1) {
        controls();
    }
}

// Initialization function to set up the Game Boy
void init() {
    // Turn off the display to avoid graphical artifacts during initialization
    DISPLAY_OFF;
    // Turn on the sound and set the volume to maximum for both channels
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    // Enable channel 1 on both left and right speakers
    NR51_REG = 0x11;
    // Turn on the display and show the background layer
    SHOW_BKG;
    DISPLAY_ON;
}

// Function to display graphics on the Game Boy
void graphics() {
    // Fill the background layer with a solid color
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0);
    // Load the native tile data for the GBDK 2020 logo
    set_native_tile_data(0, ionq_logo_TILE_COUNT, ionq_logo_tiles);
    // Set the tile map for the logo in the center of the screen
    set_tile_map((DEVICE_SCREEN_WIDTH - (ionq_logo_WIDTH >> 3)) >> 1,
                 (DEVICE_SCREEN_HEIGHT - (ionq_logo_HEIGHT >> 3)) >> 1,
                 ionq_logo_WIDTH >> 3,
                 ionq_logo_HEIGHT >> 3,
                 ionq_logo_map);
}

// Function to play audio on the Game Boy
void audio() {
    // Set various registers to play a tone on channel 1
    NR10_REG = 0x16; // frequency sweep settings
    NR11_REG = 0x40; // sound length and wave pattern duty
    NR12_REG = 0x73; // envelope settings
    NR13_REG = 0x00; // lower 8 bits of frequency
    NR14_REG = 0x8F; // higher 3 bits of frequency and enable channel 1
}

// Function to check for user input on the Game Boy
void controls() {
    // Check if any button is pressed
    if (joypad()) {
        // Play audio again if a button is pressed
        audio();
    }
    // Update the screen and wait for the next frame
    wait_vbl_done();
}
