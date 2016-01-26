#include "bh-badge.h"

void clearBuffer(void);
void writeBuffer(uint8_t x, uint8_t y, uint8_t state);
void showBuffer(void);
void putChar(uint8_t x, uint8_t y, uint8_t letter);
void showTextSlice(uint8_t startSlice, uint8_t *msg);
void initHorizontalScroll(void);
void advanceHorizontalScroll(void);
void initFiller(void);
void advanceFiller(void);
void initHatching(void);
void showHatching(void);
void initChaser();
void advanceChaser();
void animateBadge(void);
