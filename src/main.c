#include <gb/gb.h>     // import Game Boy header file for standard I/O
#include <stdbool.h>   // import types like boolean data type
#include "ionq_logo.h" // import custom header file for ionq logo graphics

static void initializeGameBoy();   // initialize the Game Boy hardware
static void displayLogoGraphics(); // display the IonQ logo graphics
void playSound();                  // play sound
void setBackgroundOffsets(bool);   // reset background offsets
static void handleUserInput();     // handle user input controls
static char rand(short);           // use assembly-optimized rand

void main()
{
    // Step 1: Initialize the Game Boy
    initializeGameBoy();
    // Step 2: Display graphics
    displayLogoGraphics();
    // Step 3: Play audio
    playSound();
    // Step 4: Reset background offsets
    setBackgroundOffsets(true);
    // Step 5: Check for user input
    while (true)
    {
        handleUserInput();
        wait_vbl_done();
    }
}

static void initializeGameBoy()
{
    DISPLAY_OFF;     // turn off display
    NR52_REG = 0x80; // sound on
    NR50_REG = 0x77; // set volume
    NR51_REG = 0x11; // enable sound channels
    SHOW_BKG;        // show background layer
    DISPLAY_ON;      // turn on display
}

static void displayLogoGraphics()
{
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0); // fill screen with black background
    set_native_tile_data(0, ionq_logo_TILE_COUNT, ionq_logo_tiles);    // set tile data for ionq logo
    set_tile_map(
        (DEVICE_SCREEN_WIDTH - (ionq_logo_WIDTH >> 3)) >> 1,   // center ionq logo horizontally
        (DEVICE_SCREEN_HEIGHT - (ionq_logo_HEIGHT >> 3)) >> 1, // center ionq logo vertically
        ionq_logo_WIDTH >> 3,                                  // set tile map width
        ionq_logo_HEIGHT >> 3,                                 // set tile map height
        ionq_logo_map);                                        // set tile map for ionq logo
}

void playSound()
{
    NR10_REG = 0x16; // set sweep register
    NR11_REG = 0x40; // set sound length/wave pattern duty
    NR12_REG = 0x73; // set envelope
    NR13_REG = 0x00; // set frequency
    NR14_REG = 0x8F; // set sound length and mode
}

void setBackgroundOffsets(bool reset)
{
    if (!reset)
    {
        SCX_REG = rand(DIV_REG); // set horizontal background offset to random number
        SCY_REG = rand(SCX_REG); // set vertical background offset to random number
    }
    else
    {
        SCX_REG = 0; // set horizontal background offset to 0
        SCY_REG = 0; // set vertical background offset to 0
    }
}

static void handleUserInput()
{
    unsigned char input = joypad();
    if (input & (J_A | J_B))                        // if A or B button is pressed
        playSound();                                // play audio
    if (input & (J_UP | J_DOWN | J_LEFT | J_RIGHT)) // if directional buttons are pressed
        setBackgroundOffsets(false);                // set background offsets to random numbers
    if (input & (J_START | J_SELECT))               // if menu buttons are pressed
        setBackgroundOffsets(true);                 // reset background offsets
}
