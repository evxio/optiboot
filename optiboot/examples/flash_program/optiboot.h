
/* Optiboot 'header' file.
 * 
 * June 2015 by Marek Wodzinski (majekw)
 * Released to public domain
 * 
 * This header file gives possibility to use SPM instruction
 * from Optiboot bootloader memory.
 *
 * There are 3 convinient functions available here:
 *  optiboot_page_erase - to erase FLASH page
 *  optiboot_page_fill - to put words into temporary buffer
 *  optiboot_page_write - to write contents of temporary buffer into FLASH
 *
 * For some hardcore users, you could use 'do_spm' as raw entry to
 * bootloader spm function.
 */

#ifndef _OPTIBOOT_H_
#define _OPTIBOOT_H_  1

#include <avr/boot.h>

/* 
 * Main 'magic' function - enter to bootloader do_spm function
 *
 * Contents of bootloader's do_spm function, just for reference:
 *
 * static void do_spm(uint16_t address, uint8_t command, uint16_t data) {
 *     asm volatile (
 *         "    movw  r0, %3\n"
 *         "    out %0, %1\n"
 *         "    spm\n"
 *         "    clr  r1\n"
 *         :
 *         : "i" (_SFR_IO_ADDR(__SPM_REG)),
 *           "r" ((uint8_t)command),
 *           "z" ((uint16_t)address),
 *           "r" ((uint16_t)data)
 *         : "r0"
 *     );
 *     if (command != __BOOT_PAGE_FILL)
 *       boot_spm_busy_wait();
 * #if defined(RWWSRE)
 *     if (command == __BOOT_PAGE_WRITE)
 *       // Reenable read access to flash
 *       boot_rww_enable();
 * #endif
 * }
 *
 */
typedef void (*do_spm_t)(uint16_t address, uint8_t command, uint16_t data);
const do_spm_t do_spm = (do_spm_t)((FLASHEND-511+2)>>1);


/*
 * The same as do_spm but with disable/restore interrupts state
 * required to succesfull SPM execution
 */
void do_spm_cli(uint16_t address, uint8_t command, uint16_t data) {
  uint8_t sreg_save;

  sreg_save = SREG;  // save old SREG value
  asm volatile("cli");  // disable interrupts
  do_spm(address,command,data);
  SREG=sreg_save; // restore last interrupts state
}


/*
 * Erase page in FLASH
 */
void optiboot_page_erase(uint16_t address) {
  do_spm_cli(address,__BOOT_PAGE_ERASE,0);
}


/*
 * Write word into temporary buffer
 */
void optiboot_page_fill(uint16_t address, uint16_t data) {
  do_spm_cli(address,__BOOT_PAGE_FILL,data);
}


/*
 * Write temporary buffer into FLASH
 */
void optiboot_page_write(uint16_t address) {
  do_spm_cli(address,__BOOT_PAGE_WRITE,0);
}

#endif /* _OPTIBOOT_H_ */

