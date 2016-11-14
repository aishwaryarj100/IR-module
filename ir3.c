#include <board.h>
#include <lcd.h>
#include <stdbool.h>
#include <stdio.h>
#include <irq.h>
#include <fifo.h>
#include "timer32.h"


static uint16_t key;
static fifo_t fifo_key;
static uint8_t fifo_buf[20];


volatile uint32_t * dir = (uint32_t *) 0x50018000;
volatile uint32_t * data = (uint32_t *) 0x50013ffc;
volatile uint32_t * config = (uint32_t *) 0x40044078;
volatile uint32_t * gpio_ie = (uint32_t *) GPIO1_IE;
volatile uint32_t * gpio_ic = (uint32_t *) GPIO1_IC;

void remote_init(void)
{
	timer32_init();
	fifo_init(&fifo_key, fifo_buf, 20);
	*config = (*config | 0x8);
	*config = (*config & ~(0x10));
	*dir = (*dir & ~(0x1));

	*gpio_ie = 1;

        irq_setcb(IRQ_PIO1, get_remote_key);

}

void get_remote_key(void)
{
	int i;
	bool laststate = 1;
	uint8_t keys_val[8] = {83, 23, 87, 84, 20, 46, 4, 68};

	
	key = 0;
	*gpio_ie = 0;
	timer32_reset();
	while(timer32_uread() < 3000) 
		;
	for (i = 0; i < 12; i++) {
		timer32_reset();
		while (timer32_uread() < 740)
			;
		if (laststate == (*data & 1)) {
			key &= ~(1 << (11 - i));
			
		} else {
			key |= (1 << (11 - i));
			timer32_reset();
			while (timer32_uread() < 600)
				;
		}
		timer32_reset();
		while (timer32_uread() < 460)
			;
		
	}
	if (key > 0) {
		printf ("%u\n", (key >> 5));
		for (i = 0 ; i < 8 ; i++)
			if (keys_val[i] == (key >> 5)) {
				fifo_add(&fifo_key, (key >> 5));
			}
	}
	*gpio_ic = 1;
	*gpio_ie = 1;
	timer32_reset();
	while(timer32_uread() < 300000) 
		;
	
}

unsigned char check_key(char diff)
{
	int flag;
	unsigned char ret = 0;
	int i;
	int j;
	uint8_t ac_key[4] = {83, 23, 87, 84};
	uint8_t remote_key[4] = {20, 46, 4 ,68};

	switch (diff) {
	case 'a':
		for (j = 0 ; j < 4 ; j++) {
			if (fifo_key.dat[fifo_key.head] == ac_key[j]) {
				ret = fifo_remove(&fifo_key);
				return ret;
			}
		}
		break;

	case 'r':
		for (j = 0 ; j < 4 ; j++) {
			if (fifo_key.dat[fifo_key.head] == remote_key[j]) {
				ret = fifo_remove(&fifo_key);
				return ret;
			}
		}
		break;
	}
	return 99;
					
}

