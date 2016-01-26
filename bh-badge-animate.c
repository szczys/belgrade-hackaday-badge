#include "bh-badge.h"
#include "font5x8.h"

#define FILLERDELAY 100
#define HORIZONTALSCROLL_DELAY  100

uint8_t testword[20] = "Hackaday";

//State definitions
typedef enum {
    NO_STATE,
    FILLER_STATE,
    HATCHING_STATE,
    CHASER_STATE,
    HORIZONTALSCROLL_STATE
} stateEnum;


uint8_t filler_i=0;
uint8_t filler_j=0;

uint32_t triggerTime = 0;
uint8_t stateFlag = NO_STATE;

uint8_t versatileCounter = 0;   //This may be used by any function

uint8_t frameBuffer[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
void clearBuffer(void) {
    for (uint8_t i=0; i<16; i++) { frameBuffer[i] = 0x00; }
}

void writeBuffer(uint8_t x, uint8_t y, uint8_t state) {
    if (state) { frameBuffer[y] |= 1<<x; }
    else { frameBuffer[y] &= ~(1<<x); }
}
    
void showBuffer(void) {
    for (uint8_t row=0; row<16; row++) {
        for (uint8_t col=0; col<8; col++) {
            if (frameBuffer[row] & 1<<col) { displayPixel(col, row, ON); }
            else { displayPixel(col, row, OFF); }
        }
    }
    displayLatch();
}

void putChar(uint8_t x, uint8_t y, uint8_t letter) {
    //H is 40
    for (uint8_t col=0; col<5; col++) {
        for (uint8_t row=0; row<7; row++) {
            if (font5x8[(letter*5)+col] & 1<<row) {
                writeBuffer(col+x, row+y, ON);
            }
            else {
                writeBuffer(col+x, row+y, OFF);
            }
        }
    }
}

uint8_t getMessageLen(uint8_t *msg) {
    //find length of string:
    uint8_t i;
    for (i=0; i<20; i++) {
        if (*(msg+i) == 0) { break; }
    }
    return i;
}

void showTextSlice(uint8_t startSlice, uint8_t *msg) {

    uint8_t colLength = getMessageLen(msg)*6;
    
    //Fill correct columns in temp buffer
    uint8_t tempBuffer[8];
    for (uint8_t i=0; i<8; i++) {
        if (startSlice>=colLength) {
            //Handle writing past the end of the last char
            tempBuffer[i] = 0x00;
            ++startSlice;
            continue;
        }
        uint8_t letter = testword[startSlice/6]-32;
        if (startSlice%6 == 5) { tempBuffer[i] = 0x00; }
        else {
            tempBuffer[i] = font5x8[(letter*5)+(startSlice%6)];
        }
        ++startSlice;
    }
    
    //Push temp buffer to framebuffer
    for (uint8_t col=0; col<8; col++) {
        for (uint8_t row=0; row<7; row++) {
            if (tempBuffer[col] & 1<<row) {
                writeBuffer(col, row, ON);
            }
            else {
                writeBuffer(col, row, OFF);
            }
        }
    }
    showBuffer();
}

void initHorizontalScroll(void) {
    versatileCounter = 0;
    clearBuffer();
    triggerTime = getTime();
    stateFlag = HORIZONTALSCROLL_STATE;
    showBuffer();
}

void advanceHorizontalScroll(void) {
    triggerTime = getTime() + HORIZONTALSCROLL_DELAY;
    showTextSlice(versatileCounter, testword);
    if (++versatileCounter >= getMessageLen(testword)*6) { versatileCounter = 0; }
}

void initFiller(void) {
    displayClear();
    triggerTime = getTime();
    stateFlag = FILLER_STATE;
    filler_i=0;
    filler_j=0;
}

void advanceFiller(void) {
    //Preload trigger time for next round
    triggerTime = getTime() + FILLERDELAY;
    
    //Manipulate the display
    displayPixel(filler_i,filler_j,ON);
    displayLatch();
    
    //Advance tracking for next round
    if (++filler_i >= TOTPIXELX) {
        filler_i = 0;
        ++filler_j;
    } 
    
    //Check to see if we've overflowed
    if (filler_j >= TOTPIXELY) {
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
    clearBuffer();
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
            case HORIZONTALSCROLL_STATE:
                if (getTime() >= triggerTime) {
                    advanceHorizontalScroll();
                }
                break;
        }
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                //initFiller();
                initHorizontalScroll();
                break;
            case (DOWN):
                //initHatching();
                stateFlag = NO_STATE;
                clearBuffer();
                /*
                uint8_t i;
                for (i=0; i<20; i++) {
                    if (testword[i] == 0) { break; }
                } 
                
                putChar(2,2,getMessageLen(&testword)+16);
                */
                showTextSlice(43, testword);
                showBuffer();
                break;
            case (RIGHT):
                initChaser();
                break;
        }
    }
}
