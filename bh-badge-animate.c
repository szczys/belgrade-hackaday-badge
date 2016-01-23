#include "bh-badge.h"

#define FILLERDELAY 100

//State definitions
#define NO_STATE        0
#define FILLER_STATE    1
#define HATCHING_STATE  2
#define SQUARE_STATE    3

uint8_t i=0;
uint8_t j=0;
uint32_t triggerTime = 0;
uint8_t stateFlag = NO_STATE;

void startFiller(void) {
    displayClear();
    triggerTime = getTime();
    stateFlag = FILLER_STATE;
    i=0;
    j=0;
}

void advanceFiller(void) {
    //Preload trigger time for next round
    triggerTime = getTime() + FILLERDELAY;
    
    //Manipulate the display
    displayPixel(i,j,ON);
    displayLatch();
    
    //Advance tracking for next round
    if (++i >= TOTPIXELX) {
        i = 0;
        ++j;
    } 
    
    //Check to see if we've overflowed
    if (j >= TOTPIXELY) {
        stateFlag = NO_STATE;
    }
}

void showHatching(void) {
    displayClear();
    for (uint8_t x=0; x<TOTPIXELX; x++) {
        for (uint8_t y=0; y<TOTPIXELY; y++) {
            if (((x%2) + (y%2)) == 1) { displayPixel(x, y, ON); }
        } 
    }
    displayLatch();
}

void showSquare(void) {
    displayClear();
    for (uint8_t x=2; x<6; x++) {
        for (uint8_t y=6; y<10; y++) {
            displayPixel(x, y, ON);
        } 
    }
    displayLatch();
}

void animateBadge(void) {
    startFiller();
    while(1) {
        switch (stateFlag) {
            case FILLER_STATE:
                if (getTime() >= triggerTime) {
                    advanceFiller();
                }
                break;
            case HATCHING_STATE:
                showHatching();
                stateFlag = NO_STATE;
                break;
            case SQUARE_STATE:
                showSquare();
                stateFlag = NO_STATE;
                break;
        }
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                startFiller();
                break;
            case (DOWN):
                stateFlag = HATCHING_STATE;
                break;
            case (RIGHT):
                stateFlag = SQUARE_STATE;
                break;
        }
    }
}
