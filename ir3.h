/**
 * remote_init - Initialize the ir module
 * 
 * Initialize the timer,fifo and interrupt.
 * Whenever the high to low transition occur,interrupt 
   is generated.The irq_setcb call the get_remote_key
   call back function.
 *
 */  
void remote_init(void);

/**
 * get_remote_key - Read the ir signal
 *
 * Read the seven command bits and five address bits and 
   equivalent decimal value is stored in the key variable.
   The command value is added in FIFO. 
   timer32_uread produces microseconds delay.
   After getting the key value the interrupt is set again.
 *
 */

void get_remote_key(void);

/**
 * check_key - Check the key value
 *
 * @diff - Check whether the key is for specific module.
 * 
 * The character diff is used to differentiate among different modules.
 * An array contains the key mappings to a module
 * Compare the value from the queue and the array values.
 * If it matches return the key value.
 */

unsigned char check_key(char diff);
