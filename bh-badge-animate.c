#include "bh-badge.h"

#define FILLERDELAY 100

//State definitions
typedef enum {
    NO_STATE,
    FILLER_STATE,
    HATCHING_STATE,
    CHASER_STATE
} stateEnum;

uint8_t i=0;
uint8_t j=0;
uint32_t triggerTime = 0;
uint8_t stateFlag = NO_STATE;

uint8_t frameBuffer[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

uint8_t writeBuffer(uint8_t x, uint8_t y, uint8_t state) {
    if (state) { frameBuffer[y] |= 1<<x; }
    else { frameBuffer[y] &= ~(1<<x); }
}
    
uint8_t showBuffer(void) {
    for (uint8_t row=0; row<16; row++) {
        for (uint8_t col=0; col<8; col++) {
            if (frameBuffer[row] & 1<<col) { displayPixel(col, row, ON); }
            else { displayPixel(col, row, OFF); }
        }
    }
    displayLatch();
}

void initFiller(void) {
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

void initHatching(void) {
    stateFlag = HATCHING_STATE;
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

void initChaser() {
    displayClear();
    triggerTime = getTime();
    stateFlag = CHASER_STATE;
    for (uint8_t y=0; y<16; y++) {
        if ((y/2)%2 == 0) {
            writeBuffer(0, y, ON);
            writeBuffer(7, y, ON);
            //hack to do top and bottom
            if (y<7) {
                writeBuffer(y+1, 15, ON);
            }
        }
    }
    writeBuffer(3, 0, ON);
    writeBuffer(4, 0, ON);
    
    showBuffer();
}

void advanceChaser() {
    //Preload trigger time for next round
    triggerTime = getTime() + FILLERDELAY;
    
    //Save pixels which will be lost in shift
    uint8_t firstPix = (1<<0 & frameBuffer[0]);
    uint8_t lastPix = (1<<7 & frameBuffer[15]);
    
    //Shift top and bottom rows
    frameBuffer[0] >>= 1;
    frameBuffer[15] <<= 1;
    
    //Move left column down
    for (uint8_t i=15; i>1; i--) {
        if (frameBuffer[i-1] & (1<<0)) { writeBuffer(0, i, ON); }
        else { writeBuffer(0, i, OFF); }
    }
    //Fix lost pixel
    if (firstPix) { writeBuffer(0, 1, ON); }
    else { writeBuffer(0, 1, OFF); }
    
    //Move right column down
    for (uint8_t i=0; i<14; i++) {
        if (frameBuffer[i+1] & (1<<7)) { writeBuffer(7, i, ON); }
        else { writeBuffer(7, i, OFF); }
    }
    //Fix lost pixel
    if (lastPix) { writeBuffer(7, 14, ON); }
    else { writeBuffer(7, 14, OFF); }
        
    showBuffer();
}

void animateBadge(void) {
    initFiller();
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
            case CHASER_STATE:
                if (getTime() >= triggerTime) {
                    advanceChaser();
                }
                break;
        }
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                initFiller();
                break;
            case (DOWN):
                initHatching();
                break;
            case (RIGHT):
                initChaser();
                break;
        }
    }
}
