#define SYSAHBCLKCTRL 0x40048080
#define TMR32B0TCR 0x40014004
#define TMR32B0TC 0x40014008
/**
 * timer32_init - Initalize the timer.
 * 
 * Enables both timer and timer clock.
 **/
void timer32_init(void);

/**
 * timer32_read - return the timer value
 *
 * Returns the Current value of the Timer.
 * 
 * Return: Timer value
 **/
uint32_t timer32_read(void);

/**
 * timer32_reset - reset the timer.
 * 
 * reset the value to zero.
 **/
void timer32_reset(void);
