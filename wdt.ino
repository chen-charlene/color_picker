/* Get next unused CPU interrupt > start */
// input: INT counter start | output: INT counter
unsigned int getNextCPUINT(unsigned int start) {
   unsigned int tryInt = start + 1;
      while (tryInt < 32) {
         if (NVIC_GetEnableIRQ((IRQn_Type) tryInt) == 0) {
            return tryInt;
         }
      tryInt++;
   }
}

// method to iniialize watchdog to reset system if not pet within timeframe
// input: NA | output: NA
void initWDT () {
  // cks
  R_WDT->WDTCR = (0b1000 << R_WDT_WDTCR_CKS_Pos) |
    (0b11 << R_WDT_WDTCR_RPES_Pos) | (0b11 << R_WDT_WDTCR_RPSS_Pos) | (0b10 << R_WDT_WDTCR_TOPS_Pos);

  // watchdog trigger interrupt
  R_WDT->WDTRCR = (0b1 << 7);
  R_ICU->IELSR[WDT_INT] = (0x025 << R_ICU_IELSR_IELS_Pos);
}

// method to pet watchdog to reset timer
// input: NA | output: NA
void petWDT() {
  // Serial.println("PET DOG");
  R_WDT-> WDTRR = 0x00;
  R_WDT-> WDTRR = 0xff;
}


