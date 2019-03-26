#ifndef __APP_GPIO_DIDO_H
#define __APP_GPIO_DIDO_H

#include "bsp_mcu_gpio.h"

void dido_gpio_init(void);
int readinput(int argc, char **argv);
int output(int argc, char **argv);
int rgb(int argc, char **argv);
int beep(int argc, char **argv);

uint8_t getChInput(uint8_t channel);
void setChOutput(uint8_t channel, uint8_t setval);

#endif
