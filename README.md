SD Card Logger for USART.
====

Types of supported SD Cards:
---
FAT12, FAT16 and FAT32.


File System:
---
The file system used on the MMC/SDC is FAT. The MMC/SDC specifications define the FAT type as: FAT12 for 64MB and smaller, FAT16 for 128MB to 2GB, FAT32 for 4GB to 32GB and exFAT for 64GB to 2TB. Only an FAT volume can be exist on the card with FDISK partitioning and no patition table like floppy disk is not allowed. Of course any file system and partitioning other than the MMC/SDC specifications define can be used as generic storage media for PCs. However such the cards with illigal format will not be accepted by DSCs, camcorders and TVs.

The USART configuration is specified in a file named: config.ini
---

The configuration entry must be in the form:
----
```c++
	0
	8-N-1
	1024
	1
```
  where:
  ----
```c++
	0 - Baud rate UART(0 - Auto selected from 600 to 115200)

	8-N-1   8 - Data bits(8,9),
			N - Parity(N - none, E - even, O - odd),
			1 - Stop bits(0.5, 1, 1.5, 2)
			
	1024 - Maximum log file size in KB(from 1 to 102400)
	
	1 - Record time in the log(1 - set time, 0 - no)
```

Supported USART interface baud rate:
----
	600,
	1200,
	2400,
	4800,
	9600,
	14400,
	19200,
	28800,
	38400,
	56000,
	57600,
	115200

Links:
----
http://elm-chan.org/fsw/ff/00index_e.html
