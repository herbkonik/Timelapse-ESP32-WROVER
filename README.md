# Timelapse-ESP32-WROOM
Take a pic each x secound and save it to the boards SD card.


The sketch is made for ESP32S3-WROOM Board / OV2640 camera, based at the sample code which you can download at www.freenove.com and should work at similar Boards to (untested). 
Without changes it should NOT work at AI Thinker and other Boards with a limited amount of pins, because i add a signal for an external flash at GPO47.

Storing reads out the last picture number at the folder and continues with the next number after a power down.
