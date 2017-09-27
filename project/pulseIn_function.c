unsigned long(GPIO_T *port, uint32_t u32PinMask, uint32_t state, unsigned long timeout) {
    uint32_t stateMask = (state ? u32PinMask : 0);         //u32PinMask = bit 
    unsigned long width = 0;            //keep initial width = 0

    //convert timeout from microsecond to a number of times through
    //the initial loop take 16 cycle per iteration
    unsigned long num_loop = 0;
    unsigned long max_loop = microsecondsToClockCycle(timeout)/16;

    while(bit == stateMask)
        if(num_loop++ == max_loop)
            return 0;

    while(bit != stateMask)
        if(num_loop++ == max_loop)
            return 0;

    while(bit == stateMask) {
        if(num_loop++ == max_loop)
            return 0;
        width++;
    }

    return clockCycleToMicrosecond(width*21+16);
}

void microsecondToClockCycle(unsigned long time) {     
    int clockCycle;
    clockCycle = (0.04*time);
    return clockCycle;
}

void clockCycleToMicrosecond(unsigned long clockCycle) {
    int microsecond;
    microsecond = (clockCycle/0.04);
    return microsecond;
}




