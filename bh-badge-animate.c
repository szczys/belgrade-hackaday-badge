#include "bh-badge.h"

#define FILLERDELAY 100

//State definitions
typedef enum {
    NO_STATE,
    FILLER_STATE,
    HATCHING_STATE,
    SQUARE_STATE,
    CHASER_STATE
};

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

void initChaser() {
    displayClear();
    triggerTime = getTime();
    stateFlag = CHASER_STATE;
    for (uint8_t y=0; y<15; y++) {
        if (y%3 == 0) {
            writeBuffer(0, y-1, ON);
            writeBuffer(7, y, ON);
            //hack to do top and bottom
            if (y<7) {
                writeBuffer(y+1, 0, ON);
                writeBuffer(y, 14, ON);
            }
        }
    }

    showBuffer();
}

void advanceChaser() {
    //Preload trigger time for next round
    triggerTime = getTime() + FILLERDELAY;
    
    //Chaser will only be in rows 0/14 and cols 0/7
    
    //Save pixels which will be lost in shift
    uint8_t firstPix = (1<<0 & frameBuffer[0]);
    uint8_t lastPix = (1<<7 & frameBuffer[14]);
    
    //Shift top and bottom rows
    frameBuffer[0] >>= 1;
    frameBuffer[14] <<= 1;
    
    //Move left column down
    for (uint8_t i=14; i>1; i--) {
        if (frameBuffer[i-1] & (1<<0)) { writeBuffer(0, i, ON); }
        else { writeBuffer(0, i, OFF); }
    }
    //Fix lost pixel
    if (firstPix) { writeBuffer(0, 1, ON); }
    else { writeBuffer(0, 1, OFF); }
    
    //Move right column down
    for (uint8_t i=0; i<13; i++) {
        if (frameBuffer[i+1] & (1<<7)) { writeBuffer(7, i, ON); }
        else { writeBuffer(7, i, OFF); }
    }
    //Fix lost pixel
    if (lastPix) { writeBuffer(7, 13, ON); }
    else { writeBuffer(7, 13, OFF); }
        
    showBuffer();
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
                startFiller();
                break;
            case (DOWN):
                stateFlag = HATCHING_STATE;
                break;
            case (RIGHT):
                //stateFlag = SQUARE_STATE;
                initChaser();
                break;
        }
    }
}
