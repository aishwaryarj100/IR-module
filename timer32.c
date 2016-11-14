#include <stdint.h>
#include "timer32.h"

void timer32_init(void)
{
	volatile uint32_t * sysclk_ctrl = (uint32_t *) SYSAHBCLKCTRL;
	volatile uint32_t * tmr32_tcr   = (uint32_t *) TMR32B0TCR;

	*sysclk_ctrl |= (1 << 9);
	*tmr32_tcr |= 1;
}

uint32_t timer32_read(void)
{
	volatile uint32_t * tmr32_tc    = (uint32_t *) TMR32B0TC;

	return (*tmr32_tc);
}

void timer32_reset(void)
{
	volatile uint32_t * tmr32_tc    = (uint32_t *) TMR32B0TC;

	*tmr32_tc = 0x0;
}

