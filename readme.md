# Hackaday | Belgrade Conference Badge

---

This badge has an 8x16 red LED matrix on it. This is project is a set of alternate firmware for it.

## Hardware Emulation

---

I'm currenlty developing code an emulator of the display I made using SDL2. It's simply a grid of circles actings as the pixels of the LED display.

### To run the game emulator

Install the SDL2 and SDL2_gfx libraries:

```sudo apt-get install libsdl2-dev```
```sudo apt-get install libsdl2-gfx-dev```

Launch the emulator by typing `make`
