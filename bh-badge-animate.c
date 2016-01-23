#include "bh-badge.h"

#define FILLERDELAY 100

uint8_t i=0;
uint8_t j=0;
uint32_t triggerTime = 0;
uint8_t fillerFlag = 0;

void startFiller(void) {
    triggerTime = getTime();
    fillerFlag = 1;
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
        fillerFlag = 0;
    }
}

void animateBadge(void) {
    startFiller();
    while(1) {
        if (fillerFlag) {
            if (getTime() >= triggerTime) {
                advanceFiller();
            }
        }
        if (getControl() == ESCAPE) {
            displayClose();
            return;
        }
    }
}
