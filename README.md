This program uses 2 ESP32s in Arduino and Amazon's AWS. Device 1 has a temperature sensor and a photosensitve resistor to detect different light levels (ambient, dark, light). Device 2 has a 10 bar bargraph LED and a pushbutton. When Device 1 makes a measurement, it will be sent to Device 2 and change the bargraph LED based on the temperature and the on board RGB LED on Device 1 and Device 2. When the user at Device 2 reads this message, the user can press the push button so that a message can appear on the Device 1 console that the user has read the message on Device 2. 