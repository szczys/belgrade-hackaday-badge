#include "bh-badge.h"

void clearBuffer(void);
void writeBuffer(uint8_t x, uint8_t y, uint8_t state);
void showBuffer(void);
void putChar(uint8_t x, uint8_t y, uint8_t letter);
void showTextSlice(int16_t startSlice, uint8_t yOffset, uint8_t *msg);
void showVertTextSlice(int16_t startSlice, uint8_t horizOffset, uint8_t *msg);
void initHorizontalScroll(void);
void advanceHorizontalScroll(void);
void initVerticalScroll(void);
void advanceVerticalScroll(void);
void initFiller(void);
void advanceFiller(void);
void initHatching(void);
void showHatching(void);
void initChaser();
void advanceChaser();
void animateBadge(void);
