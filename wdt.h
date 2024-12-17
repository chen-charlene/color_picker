unsigned int getNextCPUINT(unsigned int start);
unsigned int TIMER_INT;
unsigned int WDT_INT;

// initialize watchdog
void initWDT();

// pet watchdog to disable reset
void petWDT();

