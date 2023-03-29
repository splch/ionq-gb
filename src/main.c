#include <gb/gb.h>     // import Game Boy header file for standard I/O
#include <stdio.h>     // printf function
#include <stdlib.h>    // to dyamically allocate arrays
#include <stdbool.h>   // import types like boolean data type
#include "ionq_logo.h" // import custom header file for ionq logo graphics
#include "sprite.h"    // import sprite data

static void initializeGameBoy();   // initialize the Game Boy hardware
static void displayLogoGraphics(); // display the IonQ logo graphics
void playSound();                  // play sound
void spawnSprite();                // creates a sprite
void setBackgroundOffsets(bool);   // reset background offsets
static void handleUserInput();     // handle user input controls
static char rand(short);           // use assembly-optimized rand

unsigned int *spritePositions; // store sprite locations
unsigned int spriteCount;      // keep track of current sprite count

void main()
{
    // step 1: initialize the Game Boy
    initializeGameBoy();
    // step 2: display graphics
    displayLogoGraphics();
    // step 3: play audio
    playSound();
    // step 4: reset background offsets
    setBackgroundOffsets(true);
    // step 5: check for user input
    while (true)
    {
        handleUserInput();
        wait_vbl_done();
    }

    // free memory allocated for array before the program exits
    free(spritePositions);
}

static void initializeGameBoy()
{
    DISPLAY_OFF;                   // turn off display
    NR52_REG = 0x80;               // sound on
    NR50_REG = 0x77;               // set volume
    NR51_REG = 0x11;               // enable sound channels
    SPRITES_8x8;                   // set sprite size to 8x8
    set_sprite_data(0, 1, sprite); // load sprite data into VRAM
    spriteCount = 0;               // set the number of sprites
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
    move_sprite(spriteCount, SPRITE_CENTER_X, SPRITE_CENTER_Y); // move sprite to the center of the screen

    // update spritePositions array
    spritePositions = realloc(spritePositions, 2 * (spriteCount + 1) * sizeof(int));
    spritePositions[2 * spriteCount] = SPRITE_CENTER_X;
    spritePositions[2 * spriteCount + 1] = SPRITE_CENTER_Y;

    spriteCount++; // increment sprite count
    if (spriteCount > 40)
        spriteCount = 0;
}

void moveSprites(unsigned char input)
{
    int dx = 0;
    int dy = 0;
    if (input & J_UP)
        dy = -8;
    if (input & J_DOWN)
        dy = 8;
    if (input & J_LEFT)
        dx = -8;
    if (input & J_RIGHT)
        dx = 8;
    for (unsigned int i = 0; i < spriteCount; i++)
    {
        int newX = spritePositions[2 * i] + dx;
        int newY = spritePositions[2 * i + 1] + dy;

        spritePositions[2 * i] = newX;
        spritePositions[2 * i + 1] = newY;

        move_sprite(i, newX, newY);
    }
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
