Luke Woodhead
PSA2 CSCI 3753

pa2_char_driver.c
	The lkm we use to create our device driver, contains a memory buffer that we can read from and write to that is loaded in with the module and deleted when it is removed

test_pa2_char_driver.c
	Test function to test read and writing to the device from user space, takes it read, write and seek commands

Makefile
	Makes the .ko lkm from the device driver file