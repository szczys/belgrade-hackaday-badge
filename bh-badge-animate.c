#include "bh-badge.h"
#include "font5x8.h"

#define FILLERDELAY 100
#define HORIZONTALSCROLL_DELAY  100
#define VERTICALSCROLL_DELAY  100
#define BOUNCEBALL_DELAY    100
#define MANY0_DELAY         100
#define MANY1_DELAY         80
#define MANY2_DELAY         120
#define MANY3_DELAY         60

#define SCREENPIXELS_X  8
#define SCREENPIXELS_Y  16


uint8_t testword[20] = "Hackaday Belgrade";

//State definitions
#define NO_STATE                0
#define FILLER_STATE            1
#define HATCHING_STATE          2
#define CHASER_STATE            3
#define HORIZONTALSCROLL_STATE  4
#define VERTICALSCROLL_STATE    5
#define BOUNCEBALL_STATE        6
#define BOUNCEPATTERN_STATE     7
#define MANYBOUNCERS_STATE      8
#define TOTAL_NUMBER_OF_STATES  9


uint8_t versatile_i=0;
uint8_t versatile_j=0;
uint8_t versatile_dirX = 1;
uint8_t versatile_dirY = 1;

uint32_t triggerTime = 0;
uint8_t stateFlag = NO_STATE;

int16_t versatileCounter = 0;   //This may be used by any function

typedef struct Balls {
    uint8_t x;
    uint8_t y;
    uint8_t dirX;
    uint8_t dirY;
    uint32_t nextUpdate;
} Ball;

Ball ball0;
Ball ball1;
Ball ball2;
Ball ball3;

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

void showTextSlice(int16_t startSlice, uint8_t vertOffset, uint8_t *msg) {

    uint8_t colLength = getMessageLen(msg)*6;
    
    //Fill correct columns in temp buffer
    uint8_t tempBuffer[8];
    for (uint8_t i=0; i<8; i++) {
        if ((startSlice>=colLength) || (startSlice<0)) {
            //Handle writing past the end of the last char
            tempBuffer[i] = 0x00;
            ++startSlice;
            continue;
        }
        uint8_t letter = msg[startSlice/6]-32;
        if (startSlice%6 == 5) { tempBuffer[i] = 0x00; }
        else {
            tempBuffer[i] = font5x8[(letter*5)+(startSlice%6)];
        }
        ++startSlice;
    }
    
    //Push temp buffer to framebuffer
    for (uint8_t col=0; col<8; col++) {
        for (uint8_t row=0; (row<7) && (row+vertOffset<16); row++) {
            if (tempBuffer[col] & 1<<row) {
                writeBuffer(col, row+vertOffset, ON);
            }
            else {
                writeBuffer(col, row+vertOffset, OFF);
            }
        }
    }
    showBuffer();
}

void showVertTextSlice(int16_t startSlice, uint8_t horizOffset, uint8_t *msg) {
    //Show an 8x16 slice of our pessage for vertical scrolling
    //Params:
    //  startSlice: Where we are in the message (8 rows per character)
    //  horizOffset: where the left edge of the letters will be
    //  msg: Pointer to the array that stores the message

    uint8_t rowLength = getMessageLen(msg)*8;
    
    //Fill correct columns in temp buffer
    for (uint8_t i=0; i<16; i++) {
        if ((startSlice>=rowLength) || (startSlice<0)) {
            //Handle writing past the end of the last char
            frameBuffer[i] = 0x00;
            ++startSlice;
            continue;
        }
        uint8_t letter = msg[startSlice/8]-32;
        //Force uppercase otherwise vert scrolling has too many spaced betten characters
        if ((65 <= letter) && (letter <= 90)) { letter -= 32; }
        if (startSlice%8 == 7) { frameBuffer[i] = 0x00; }
        else {
            for (uint8_t j=0; j<5; j++) {
                if (font5x8[(letter*5)+j] & (1<<startSlice%8))
                {
                    writeBuffer(j+horizOffset,i,ON);
                }
                else {
                    writeBuffer(j+horizOffset,i,OFF);
                }
            }
        }
        ++startSlice;
    }

    showBuffer();
}

void initHorizontalScroll(void) {
    versatileCounter = -7;
    clearBuffer();
    triggerTime = getTime();
    stateFlag = HORIZONTALSCROLL_STATE;
    showBuffer();
}

void advanceHorizontalScroll(void) {
    triggerTime = getTime() + HORIZONTALSCROLL_DELAY;
    showTextSlice(versatileCounter, 5, testword);
    if (++versatileCounter >= (getMessageLen(testword)*6)+8) { versatileCounter = -7; }
}

void initVerticalScroll(void) {
    versatileCounter = -7;
    clearBuffer();
    triggerTime = getTime();
    stateFlag = VERTICALSCROLL_STATE;
    showBuffer();
}

void advanceVerticalScroll(void) {
    triggerTime = getTime() + VERTICALSCROLL_DELAY;
    showVertTextSlice(versatileCounter, 2, testword);
    if (++versatileCounter >= (getMessageLen(testword)*8)+16) { versatileCounter = -16; }
}

void initFiller(void) {
    displayClear();
    triggerTime = getTime();
    stateFlag = FILLER_STATE;
    versatile_i=0;
    versatile_j=0;
}

void advanceFiller(void) {
    //Preload trigger time for next round
    triggerTime = getTime() + FILLERDELAY;
    
    //Manipulate the display
    displayPixel(versatile_i,versatile_j,ON);
    displayLatch();
    
    //Advance tracking for next round
    if (++versatile_i >= TOTPIXELX) {
        versatile_i = 0;
        ++versatile_j;
    } 
    
    //Check to see if we've overflowed
    if (versatile_j >= TOTPIXELY) {
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

uint8_t initBounceBall(struct Balls* thisBall, uint8_t delay ) {
    clearBuffer();
    stateFlag = BOUNCEBALL_STATE;
    thisBall->nextUpdate = getTime() + delay;
    thisBall->x = 0;
    thisBall->y = 0;
    writeBuffer(thisBall->x, thisBall->y, ON);
    thisBall->dirX = 1;
    thisBall->dirY = 1;
    showBuffer();
}

uint8_t advanceBounceBall(struct Balls* thisBall, uint8_t delay, uint8_t toggle) {
    thisBall->nextUpdate = getTime() + delay;
    
    uint8_t oldX = thisBall->x;
    uint8_t oldY = thisBall->y;
    
    //move ball
    if (thisBall->dirX) {
        if (++thisBall->x >= SCREENPIXELS_X) {
            thisBall->dirX = 0;
            thisBall->x -= 2;
        }
    }
    else {
        if (thisBall->x == 0) {
            thisBall->dirX = 1;
            ++thisBall->x;
        }
        else { --thisBall->x; }
    }
    if (thisBall->dirY) {
        if (++thisBall->y >= SCREENPIXELS_Y) {
            thisBall->dirY = 0;
            thisBall->y -= 2;
        }
    }
    else {
        if (thisBall->y == 0) {
            thisBall->dirY = 1;
            ++thisBall->y;
        }
        else { --thisBall->y; }
    }

    if (toggle) {
        if (frameBuffer[thisBall->y] & 1<<thisBall->x) { writeBuffer(thisBall->x, thisBall->y, OFF); }
        else { writeBuffer(thisBall->x, thisBall->y, ON); }
    }
    else {
        //unset ball in buffer
        writeBuffer(oldX, oldY, OFF);
        //redraw ball in new loaction in buffer
        writeBuffer(thisBall->x, thisBall->y, ON);
    }
    //show buffer
    showBuffer();
}

void initManyBouncers(void) {
    clearBuffer();
    stateFlag = MANYBOUNCERS_STATE;
    
    ball0.nextUpdate = getTime() + MANY0_DELAY;
    ball1.nextUpdate = getTime() + MANY1_DELAY;
    ball2.nextUpdate = getTime() + MANY2_DELAY;
    ball3.nextUpdate = getTime() + MANY3_DELAY;
    
    ball0.x = 0;
    ball0.y = 0;
    writeBuffer(ball0.x, ball0.y, ON);
    ball0.dirX = 1;
    ball0.dirY = 1;
    
    ball1.x = 4;
    ball1.y = 14;
    writeBuffer(ball1.x, ball1.y, ON);
    ball1.dirX = 0;
    ball1.dirY = 1;
    
    ball2.x = 3;
    ball2.y = 6;
    writeBuffer(ball2.x, ball2.y, ON);
    ball2.dirX = 1;
    ball2.dirY = 0;
    
    ball3.x = 6;
    ball3.y = 9;
    writeBuffer(ball3.x, ball3.y, ON);
    ball3.dirX = 1;
    ball3.dirY = 1;
    
    showBuffer();
}

void advanceState(void) {
    //Increments to the next state
    if(++stateFlag >= TOTAL_NUMBER_OF_STATES) { stateFlag = 1; }
    
    switch (stateFlag) {
        case FILLER_STATE:
            initFiller();
            break;
        case HATCHING_STATE:
            showHatching();
            break;
        case CHASER_STATE:
            initChaser();
            break;
        case HORIZONTALSCROLL_STATE:
            initHorizontalScroll();
            break;
        case VERTICALSCROLL_STATE:
            initVerticalScroll();
            break;
        case BOUNCEBALL_STATE:
            initBounceBall(&ball0, BOUNCEBALL_DELAY);
            break;
        case BOUNCEPATTERN_STATE:
            initBounceBall(&ball0, BOUNCEBALL_DELAY);
            stateFlag = BOUNCEPATTERN_STATE;
            break;
        case MANYBOUNCERS_STATE:
            initManyBouncers();
            stateFlag = MANYBOUNCERS_STATE;
            break;
    }
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
            case VERTICALSCROLL_STATE:
                if (getTime() >= triggerTime) {
                    advanceVerticalScroll();
                }
                break;
            case BOUNCEBALL_STATE:
                if (getTime() >= ball0.nextUpdate) {
                    advanceBounceBall(&ball0, BOUNCEBALL_DELAY, 0);
                }
                break;
            case BOUNCEPATTERN_STATE:
                if (getTime() >= ball0.nextUpdate) {
                    advanceBounceBall(&ball0, BOUNCEBALL_DELAY, 1);
                }
                break;
            case MANYBOUNCERS_STATE:
                if (getTime() >= ball0.nextUpdate) {
                    advanceBounceBall(&ball0, MANY0_DELAY, 0);
                }
                if (getTime() >= ball1.nextUpdate) {
                    advanceBounceBall(&ball1, MANY1_DELAY, 0);
                }
                if (getTime() >= ball2.nextUpdate) {
                    advanceBounceBall(&ball2, MANY2_DELAY, 0);
                }
                if (getTime() >= ball3.nextUpdate) {
                    advanceBounceBall(&ball3, MANY3_DELAY, 0);
                }
                break;
        }
        switch (getControl()) {
            case (ESCAPE):
                displayClose();
                return;
            case (LEFT):
                advanceState();
                break;
            case (DOWN):
                initVerticalScroll();
                break;
            case (RIGHT):
                initChaser();
                break;
        }
    }
}
