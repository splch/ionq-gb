/*
Game Boy code for IonQ's lunch-n-learn

This code demonstrates the following functionalities:
    1. Graphics rendering
    2. Audio playback
    3. User input handling
    4. Micro optimizations
*/

#include <gb/gb.h>     // import Game Boy header file for standard I/O
#include <stdlib.h>    // to dyamically allocate arrays
#include <stdbool.h>   // import types like boolean data type
#include "ionq_logo.h" // import custom header file for ionq logo graphics
#include "sprite.h"    // import sprite data

unsigned int spritePositions[2 * MAX_HARDWARE_SPRITES + 1]; // store sprite locations
unsigned int spriteCount;                                   // keep track of current sprite count
unsigned int currentFrame;                                  // used to alternate sprite visibility

static void initializeGameBoy();          // initialize the Game Boy hardware
static void displayLogoGraphics();        // display the IonQ logo graphics
void playSound();                         // play sound
static void spawnSprite();                // creates a sprite
static void flickerSprites(unsigned int); // surpass the sprite per line limit
static void moveSprites(unsigned char);   // moves all sprites by d-pad
void setBackgroundOffsets(bool);          // reset background offsets
static void handleUserInput();            // handle user input controls
extern char rand(short);                  // use assembly-optimized rand

void main()
{
    initializeGameBoy();        // step 1: initialize the Game Boy
    displayLogoGraphics();      // step 2: display graphics
    playSound();                // step 3: play audio
    setBackgroundOffsets(true); // step 4: reset background offsets

    while (true)
    {
        handleUserInput();              // step 5: check for user input
        flickerSprites(currentFrame++); // step 6: micro optimizations
        wait_vbl_done();
    }
}

static void initializeGameBoy()
{
    DISPLAY_OFF;                   // turn off display
    SPRITES_8x8;                   // set sprite size to 8x8
    set_sprite_data(0, 1, sprite); // load sprite data into VRAM
    spriteCount = 0;               // set the number of sprites
    currentFrame = 0;              // set the first frame to 0
    NR52_REG = 0x80;               // sound on
    NR50_REG = 0x77;               // set volume
    NR51_REG = 0x11;               // enable sound channels
    SHOW_SPRITES;                  // show sprite layer
    SHOW_BKG;                      // show background layer
    DISPLAY_ON;                    // turn on display
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

void spawnSprite()
{
    set_sprite_tile(spriteCount, 0);                                          // set sprite tile to first in vram
    set_sprite_prop(spriteCount, get_sprite_prop(spriteCount) & ~S_PRIORITY); // make initially sprite visible
    move_sprite(spriteCount, SPRITE_CENTER_X, SPRITE_CENTER_Y);               // Move the sprite to the center of the screen

    spritePositions[2 * spriteCount] = SPRITE_CENTER_X;     // put new sprite in center x
    spritePositions[2 * spriteCount + 1] = SPRITE_CENTER_Y; // and center y

    if (++spriteCount > MAX_HARDWARE_SPRITES) // if increased sprite count is over max
        spriteCount = 0;                      // reset to 0 to avoid seg faults
}

void flickerSprites(unsigned int frame)
{
    unsigned int startSprite = frame % 2 == 0 ? 0 : MAX_HARDWARE_SPRITES; // determine visible sprites

    for (unsigned int i = 0; i < MAX_HARDWARE_SPRITES; i++)
    {
        int index = (startSprite + i) % spriteCount; // select sprite from visible group
        int x = spritePositions[2 * index];          // get its x
        int y = spritePositions[2 * index + 1];      // and y location
        move_sprite(i, x, y);                        // move it back
    }
}

void moveSprites(unsigned char input)
{
    int dx = 0;          // set the difference of x
    int dy = 0;          // and y
    if (input & J_UP)    // if the user pressed up
        dy = -8;         // move the sprite up one tile
    if (input & J_DOWN)  // down
        dy = 8;          // one tile
    if (input & J_LEFT)  // left
        dx = -8;         // one tile
    if (input & J_RIGHT) // right
        dx = 8;          // one tile

    for (unsigned int i = 0; i < MAX_HARDWARE_SPRITES; i++)
    {
        int newX = spritePositions[2 * i] + dx;     // add difference to x
        int newY = spritePositions[2 * i + 1] + dy; // and y positions
        spritePositions[2 * i] = newX;              // update sprite x
        spritePositions[2 * i + 1] = newY;          // and y location
        move_sprite(i, newX, newY);                 // then move the sprite
    }
}

void setBackgroundOffsets(bool reset)
{
    if (!reset)
    {
        SCX_REG = rand(DIV_REG); // set the horizontal
        SCY_REG = rand(SCX_REG); // and vertical background offset to random number
    }
    else
    {
        SCX_REG = 0; // set the horizontal
        SCY_REG = 0; // and vertical background offset to 0
    }
}

static void handleUserInput()
{
    unsigned char input = joypad();
    if (input & J_A)                                // if A button is pressed
        playSound();                                // play audio
    if (input & J_B)                                // if B button is pressed
        setBackgroundOffsets(false);                // set background offsets to random numbers
    if (input & (J_START | J_SELECT))               // if menu buttons are pressed
        setBackgroundOffsets(true);                 // reset background offsets
    if (input & (J_UP | J_DOWN | J_LEFT | J_RIGHT)) // if directional buttons are pressed
    {
        moveSprites(input); // move all sprites in the direction
        spawnSprite();      // generate a sprite
    }
}
