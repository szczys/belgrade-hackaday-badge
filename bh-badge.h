#include <inttypes.h>

#define TOTPIXELX       8
#define TOTPIXELY       16

//LED Control definitions
#define OFF     0
#define ON      1

#ifdef __cplusplus
extern "C"{
#endif

//Directions of travel
#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3
//Miscellaneous
#define ESCAPE  4
#define NOINPUT 5
#define BUTTON  6


/*---- Display Prototypes ----*/
void initDisplay(void);
void displayClear(uint8_t color);
void displayGameOver(void);
void displayPixel(uint8_t x, uint8_t y, uint8_t state);
void displayClose(void);
void displayLatch(void);
/*--------------------*/



/*---- Control Prototypes ----*/
void initControl(void);
uint8_t getControl(void);
uint32_t getTime(void);
void controlDelayMs(uint16_t ms);

#ifdef __cplusplus
} // extern "C"
#endif
