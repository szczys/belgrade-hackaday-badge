#include "bh-badge.h"

uint8_t clearBuffer(void);
uint8_t writeBuffer(uint8_t x, uint8_t y, uint8_t state);
uint8_t showBuffer(void);
uint8_t putChar(uint8_t x, uint8_t y, uint8_t letter);
uint8_t showTextSlice(uint8_t startSlice);
void initFiller(void);
void advanceFiller(void);
void initHatching(void);
void showHatching(void);
void initChaser();
void advanceChaser();
void animateBadge(void);
