# Modifications

This is a modified version of the SdFat library providing a custom SdSpiDriver
for an MSP430. The Arduino library magic stuff essentially means everything
gets built, but because only the MSP430 code doesn't get optimized away,
if you define SD_HAS_CUSTOM_SPI before you include SdFat.h, the optimized
driver will be used.

The optimized driver is pretty darn fast, I haven't run benchmarks but
it's ~MB/s. It's hard coded to use EUSCI B0 and 2 DMA channels (DMA0/DMA1)
so it'll only work with boards that have those, and also ones where
the Energia SPI core maps to EUSCI B0.

# Original README

The Arduino SdFat library provides read/write access to FAT16/FAT32
file systems on SD/SDHC flash cards.

SdFat requires Arduino 1.6x or greater.

Key changes:

Support for multiple SPI ports now uses a pointer to a SPIClass object.

See the STM32Test example.
```
explicit SdFat(SPIClass* spiPort);
\\ or
explicit SdFatEX(SPIClass* spiPort);
```

Open flags now follow POSIX conventions.  O_RDONLY is the same as O_READ and O_WRONLY is the
same as O_WRITE. One and only one of of the mode flags, O_RDONLY, O_RDWR, or
O_WRONLY is required.
 
Open flags for Particle Gen3 and ARM boards are now defined by fcntl.h.
See SdFatConfig.h for options.

Support for particle Gen3 devices.

New cluster allocation algorithm.

Please read changes.txt and the html documentation in the extras folder for more information.

Please report problems as issues.

A number of configuration options can be set by editing SdFatConfig.h
define macros.  See the html documentation for details

Please read the html documentation for this library.  Start with
html/index.html and read the Main Page.  Next go to the Classes tab and
read the documentation for the classes SdFat, SdFatEX, SdBaseFile,
SdFile, File, StdioStream, ifstream, ofstream, and others.
 
Please continue by reading the html documentation in SdFat/extras/html

Updated 28 Dec 2018

