#include "bh-badge.h"

void animateBadge(void) {
    for (uint8_t i=0; i<TOTPIXELX; i++) {
        for (uint8_t j=0; j<TOTPIXELY; j++) {
            displayPixel(i,j,RED);
            displayLatch();
            controlDelayMs(100);
        }
    }
}
