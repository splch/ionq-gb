#include <gbdk/platform.h>

#include "GBDK_2020_logo.h"

void main() {
    DISPLAY_OFF;
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0);
    set_native_tile_data(0, GBDK_2020_logo_TILE_COUNT, GBDK_2020_logo_tiles);
    set_tile_map((DEVICE_SCREEN_WIDTH - (GBDK_2020_logo_WIDTH >> 3)) >> 1, 
                 (DEVICE_SCREEN_HEIGHT - (GBDK_2020_logo_HEIGHT >> 3)) >> 1, 
                 GBDK_2020_logo_WIDTH >> 3, 
                 GBDK_2020_logo_HEIGHT >> 3, 
                 GBDK_2020_logo_map);
    SHOW_BKG;
    DISPLAY_ON;
}
