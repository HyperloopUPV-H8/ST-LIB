/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
__sstack = ORIGIN(DTCMRAM);
__estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);
/* Generate a link error if heap and stack don't fit into DTCM */
__Min_Heap_Size = 0x200;  /* required amount of heap  */
__Min_Stack_Size = 0x400; /* required amount of stack */

/* Input for ITCM confugarable size, default to 64K */
__ITCM_SIZE = DEFINED(__ITCM_SIZE) ? __ITCM_SIZE : 64K;
/* Validation: ensure ITCM size is one of the allowed values */
ASSERT((__ITCM_SIZE == 64K) || (__ITCM_SIZE == 128K) || (__ITCM_SIZE == 192K) || (__ITCM_SIZE == 256K), \
  "Invalid ITCM size. Must be 64K, 128K, 192K, or 256K")

/* Specify the memory areas */
MEMORY
{
  ITCMRAM (xrw)   : ORIGIN = 0x00000000,   LENGTH = __ITCM_SIZE /* Configurable ITCM size, default to 64K */
  DTCMRAM (rw)    : ORIGIN = 0x20000000,   LENGTH = 128K
  FLASH    (xr)   : ORIGIN = 0x08000000,   LENGTH = 1024K-128K-128K
  FLASH_ST (xr)   : ORIGIN = 0x080C0000,   LENGTH = 128K
  FLASH_BT (xr)   : ORIGIN = 0x080E0000,   LENGTH = 128K
  RAM_D1  (rw)    : ORIGIN = 0x24000000,   LENGTH = 320K - (__ITCM_SIZE - 64K) /* ITCM memory is carved out of D1 RAM */
  RAM_D2  (rw)    : ORIGIN = 0x30000000,   LENGTH = 32K
  RAM_D3  (rw)    : ORIGIN = 0x38000000,   LENGTH = 16K
  PERIPHERAL (rw) : ORIGIN = 0x40000000,   LENGTH = 512M
}

/* Define output sections */
SECTIONS
{
  /* Export Memory Layout Information for MPU Configuration (dynamic symbols are defined later) */
  __itcm_base       = ABSOLUTE(ORIGIN(ITCMRAM));
  __itcm_size       = ABSOLUTE(LENGTH(ITCMRAM));
  __dtcm_base       = ABSOLUTE(ORIGIN(DTCMRAM));
  __dtcm_size       = ABSOLUTE(LENGTH(DTCMRAM));
  __flash_base      = ABSOLUTE(ORIGIN(FLASH));
  __flash_size      = ABSOLUTE(LENGTH(FLASH) + LENGTH(FLASH_ST) + LENGTH(FLASH_BT)); /* Total flash size across all regions */
  __ram_d1_base     = ABSOLUTE(ORIGIN(RAM_D1));
  __ram_d1_size     = ABSOLUTE(LENGTH(RAM_D1));
  __ram_d2_base     = ABSOLUTE(ORIGIN(RAM_D2));
  __ram_d2_size     = ABSOLUTE(LENGTH(RAM_D2));
  __ram_d3_base     = ABSOLUTE(ORIGIN(RAM_D3));
  __ram_d3_size     = ABSOLUTE(LENGTH(RAM_D3));
  __peripheral_base = ABSOLUTE(ORIGIN(PERIPHERAL));
  __peripheral_size = ABSOLUTE(LENGTH(PERIPHERAL));

  PROVIDE(_itcm_base = __itcm_base);
  PROVIDE(_itcm_size = __itcm_size);
  PROVIDE(_dtcm_base = __dtcm_base);
  PROVIDE(_dtcm_size = __dtcm_size);
  PROVIDE(_flash_base = __flash_base);
  PROVIDE(_flash_size = __flash_size);
  PROVIDE(_ram_d1_base = __ram_d1_base);
  PROVIDE(_ram_d1_size = __ram_d1_size);
  PROVIDE(_ram_d2_base = __ram_d2_base);
  PROVIDE(_ram_d2_size = __ram_d2_size);
  PROVIDE(_ram_d3_base = __ram_d3_base);
  PROVIDE(_ram_d3_size = __ram_d3_size);
  PROVIDE(_peripheral_base = __peripheral_base);
  PROVIDE(_peripheral_size = __peripheral_size);

  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */

    . = ALIGN(4);
  } >FLASH

  /* The program code goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    __stext = .;

    *(.text)          /* .text sections (code) */
    *(.text*)         /* .text* sections (code) */
    *(.glue_7)        /* glue arm to thumb code */
    *(.glue_7t)       /* glue thumb to arm code */
    *(.eh_frame)      /* Exception handling frames, mainly for the debugger */

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    __etext = .;
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)        /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)       /* .rodata* sections (constants, strings, etc.) */

    . = ALIGN(4);
  } >FLASH

  /* ARM exception handling tables go into FLASH */
  .ARM.extab (READONLY): { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM (READONLY): {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array (READONLY):
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array (READONLY):
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array (READONLY):
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* .hard_fault_log has to be the first thing in the FLASH_ST */
  .hardfault_log :
  {
    . = ALIGN(4);
    __hf_log = .;
    KEEP(*(.hardfault_log));
    . += 0x200;

    . = ALIGN(4);
    PROVIDE(_hf_log = __hf_log);
  } >FLASH_ST

  .metadata_pool :
  {
    . = ALIGN(4);
    __metadata = .;
    KEEP(*(.metadata_pool))
    . += 0x100;

    . = ALIGN(4);
    PROVIDE(_metadata = __metadata);
  } >FLASH_ST

  /* Code running in ITCM RAM (0 Wait States, Instruction Bus) */
  .ram_code :
  {
  	. = ALIGN(4);
  	__sram_code = .;
    *(.ram_code)    /* .ram_code sections, for code marked to be placed in ITCM RAM */
    *(.ram_code*)   /* .ram_code* sections, for code marked to be placed in ITCM RAM */
    
    . = ALIGN(4);
    __eram_code = .;
  } >ITCMRAM AT> FLASH
  __si_ram_code = LOADADDR(.ram_code);

  /* Stack memory to avoid memfault inside hardfault handler, at the start of DTCM */
  .hardfault_stack (NOLOAD) : 
  {
    . = ALIGN(8);
    __hf_stack_start = .;
    . += 0x400;       /* 1 KB */

    . = ALIGN(8);
    __hf_stack_end = .;
  } >DTCMRAM

  /* Initialized data sections goes into RAM, copied from FLASH */
  .data :
  {
    . = ALIGN(4);
    __sdata = .;
    *(.data)          /* .data sections */
    *(.data*)         /* .data* sections */

    . = ALIGN(4);
    __edata = .;
  } >DTCMRAM AT> FLASH
  __si_data = LOADADDR(.data);

  /* Uninitialized data section */
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __sbss = .;
    __bss_start__ = __sbss;
    *(.bss)           /* .bss sections */
    *(.bss*)          /* .bss* sections */
    *(COMMON)         /* common sections, same as the bss, mainly generated by legacy code */

    . = ALIGN(4);
    __ebss = .;
    __bss_end__ = __ebss;
  } >DTCMRAM

  .dtcm_rodata :
  {
    . = ALIGN(4);
    __sdtcm_rodata = .;
    *(.dtcm_rodata)   /* .dtcm_rodata sections, for constants marked to be placed in DTCM RAM */
    *(.dtcm_rodata*)  /* .dtcm_rodata* sections, for constants marked to be placed in DTCM RAM */

    . = ALIGN(4);
    __edtcm_rodata = .;
  } >DTCMRAM AT> FLASH
  __si_dtcm_rodata = LOADADDR(.dtcm_rodata);

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + __Min_Heap_Size;
    . = . + __Min_Stack_Size;
    
    . = ALIGN(8);
  } >DTCMRAM

  /* MPU D1 Non-Cached Section: Placed at start of RAM_D1 for alignment */
  .ram_d1_nc (NOLOAD) :
  {
    . = ALIGN(32);
    __d1_nc_start = ABSOLUTE(.);

    /* MPU system buffers */
    *(.ram_d1_nc.buffer)

    /* User manual allocations via D1_NC macro */
    . = ALIGN(32);
    *(.ram_d1_nc.user)

    /* Ethernet Rx Pool */
    . = ALIGN(32);
    *(.Rx_PoolSection)
  } >RAM_D1
  /* Calculate padding for MPU subregions (D1) */
  __d1_size = SIZEOF(.ram_d1_nc);
  /* Find next power of 2 (up to 512KB for 320KB RAM) */
  __d1_p2 = (1 << LOG2CEIL(MAX(32, __d1_size)));
  /* Subregion size is RegionSize / 8 */
  __d1_sub = __d1_p2 / 8;
  /* Align effective size to the subregion granularity */
  __d1_pad = (__d1_size + __d1_sub - 1) / __d1_sub * __d1_sub;
  /* Advance current pointer to reserve this space */
  . = __d1_nc_start + __d1_pad;
  __d1_nc_end = ABSOLUTE(.);
  PROVIDE(_ram_d1_nc_start = __d1_nc_start);
  PROVIDE(_ram_d1_nc_end = __d1_nc_end);

  /* MPU D1 Cached Section */
  .ram_d1 (NOLOAD) :
  {
    . = ALIGN(32);

    /* User manual allocations via D1_C macro */
    *(.ram_d1.user)

    /* MPU system buffers */
    *(.ram_d1.buffer)
  } >RAM_D1

  /* MPU D2 Non-Cached Section: Contains Ethernet Descriptors and Generic Buffers */
  .ram_d2_nc (NOLOAD) :
  {
    . = ALIGN(32);
    __d2_nc_start = ABSOLUTE(.);

    /* ETH Descriptors - Must be aligned */
    *(.RxDecripSection)
    *(.TxDecripSection)

    /* MPU system buffers */
    *(.ram_d2_nc.buffer)

    /* User manual allocations via D2_NC macro */
    *(.ram_d2_nc.user)
  } >RAM_D2
  /* Calculate padding for MPU subregions (D2) */
  __d2_size = SIZEOF(.ram_d2_nc);
  /* Find next power of 2 (up to 64KB for 32KB RAM) */
  __d2_p2 = (1 << LOG2CEIL(MAX(32, __d2_size)));
  __d2_sub = __d2_p2 / 8;
  __d2_pad = (__d2_size + __d2_sub - 1) / __d2_sub * __d2_sub;
  . = __d2_nc_start + __d2_pad;
  __d2_nc_end = ABSOLUTE(.);
  PROVIDE(_ram_d2_nc_start = __d2_nc_start);
  PROVIDE(_ram_d2_nc_end = __d2_nc_end);

  /* MPU D2 Cached Section */
  .ram_d2 (NOLOAD) :
  {
    . = ALIGN(32);

    /* User manual allocations via D2_C macro */
    *(.ram_d2.user)

    /* MPU system buffers */
    *(.ram_d2.buffer)
  } >RAM_D2

  /* MPU D3 Non-Cached Section */
  .ram_d3_nc (NOLOAD) :
  {
    . = ALIGN(32);
    __d3_nc_start = ABSOLUTE(.);

    /* New MPU system buffers */
    *(.ram_d3_nc.buffer)

    /* Legacy MPUManager allocations */
    *(.ram_d3_nc.legacy)

    /* User manual allocations via D3_NC macro */
    *(.ram_d3_nc.user)

  } >RAM_D3

  /* Calculate padding for MPU subregions (D3) */
  __d3_size = SIZEOF(.ram_d3_nc);
  /* Find next power of 2 (up to 32KB for 16KB RAM) */
  __d3_p2 = (1 << LOG2CEIL(MAX(32, __d3_size)));
  __d3_sub = __d3_p2 / 8;
  __d3_pad = (__d3_size + __d3_sub - 1) / __d3_sub * __d3_sub;
  . = __d3_nc_start + __d3_pad;
  __d3_nc_end = ABSOLUTE(.);
  PROVIDE(_ram_d3_nc_start = __d3_nc_start);
  PROVIDE(_ram_d3_nc_end = __d3_nc_end);

  /* MPU D3 Cached Section */
  .ram_d3 (NOLOAD) :
  {
    . = ALIGN(32);

    /* User manual allocations via D3_C macro */
    *(.ram_d3.user)

    /* New MPU system buffers */
    *(.ram_d3.buffer)
  } >RAM_D3

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) } /* Metadata, not allocated, only exists in the .elf */
}
