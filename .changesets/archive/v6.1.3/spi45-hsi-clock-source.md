release: patch
summary: Drive SPI4 and SPI5 from HSI for stable 2 MHz BMS transfers

SPI4 and SPI5 now use the 64 MHz HSI peripheral clock source instead of PCLK2,
allowing a 2 MHz request to select the /32 SPI prescaler deterministically.
