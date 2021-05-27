#ifndef __BOARD_COMMON_H__
#define __BOARD_COMMON_H__

/**
 * @brief   Magic number for jumping to bootloader.
 */
#if !defined(MAGIC_BOOTLOADER_NUMBER) || defined(__DOXYGEN__)
#define MAGIC_BOOTLOADER_NUMBER 0xDEADBEEF
#endif

#endif
