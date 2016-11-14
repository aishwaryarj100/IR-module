#include <board.h>
#include <lcd.h>
#include <stdbool.h>
#include <stdio.h>
#include <irq.h>
#include <fifo.h>
#include "timer32.h"

bool laststate = 1;
uint16_t a;
fifo_t fifo_key;
uint8_t fifo_buf[20];
uint8_t keys_val[8] = {83, 23, 87, 84, 20, 46, 4, 68};

volatile uint32_t * dir = (uint32_t *) 0x50018000;
volatile uint32_t * data = (uint32_t *) 0x50013ffc;
volatile uint32_t * config = (uint32_t *) 0x40044078;
volatile uint32_t * gpio_ie = (uint32_t *) GPIO1_IE;
volatile uint32_t * gpio_ic = (uint32_t *) GPIO1_IC;

void get_remote_key()
{
	int i;
	a = 0;
	*gpio_ie = 0;
	timer32_reset();
	while(timer32_read() < 216000) 
		;
	for (i = 0; i < 12; i++) {
		timer32_reset();
		while (timer32_read() < 53200)
			;
		if (laststate == (*data & 1)) {
			a &= ~(1 << (11 - i));
			
		} else {
			a |= (1 << (11 - i));
			timer32_reset();
			while (timer32_read() < 43200)
				;
		}
		timer32_reset();
		while (timer32_read() < 33200)
			;
		
	}
	if (a > 0) {
		printf ("%u\n", (a >> 5));
		for (i = 0 ; i < 8 ; i++)
			if (keys_val[i] == (a >> 5)) {
				fifo_add(&fifo_key, (a >> 5));
				printf("here %d\n",fifo_key.dat[fifo_key.tail - 1]);
			}
	}
	*gpio_ic = 1;
	*gpio_ie = 1;
	timer32_reset();
	while(timer32_read() < 21600000) 
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

void func_call(void)
{
	unsigned char ch;
	get_remote_key();
	ch = check_key('a');
	if (ch == 99)
		printf("err");
	else
		printf("value=%d",ch);
        
}

int main(void)
{
	int i;
	board_init();
	lcd_init();
	timer32_init();
	board_stdout(LCD_STDOUT);
	*config = (*config | 0x8);
	*config = (*config & ~(0x10));
	*dir = (*dir & ~(0x1));
	fifo_init(&fifo_key, fifo_buf, 20);

	*gpio_ie = 1;

        irq_setcb(IRQ_PIO1, func_call);

	return 0;
}
