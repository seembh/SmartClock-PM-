# SmartClock-PM

## Introduction:
### Motivation
  The project started out as a normal digital clock that was neither accurate nor smart. 
  
### Steps:
  I started by adding an RTC module and an LCD display (both using I2C) and an LED matrix which was helpful in figuring the "clock" part out. The RTC module works by using a battery to store (for an year) the date ( day / month / year) and the time ( hour / minute / second). This was implemented using the DHT11 library. The LCD was implemented using the LiquidCrystal_I2C library and it works well with another I2C device (the RTC module). For the LED matrix I used 2 libraries but it can be done with only one (MD_Parola and MD_MAX72XX). 
  The sensors I used were a distance sensor which I setup to detect a distance less than 6 cm and the PIR which I setup on a medium sensitivity (3.5m roughly) and 10 seconds delay.
  The PIR sensor uses interrupts to send a state to the Arduino.
  Another thing I added was a buzzer that uses a potentiometer to control the volume. Because of the high current draw of the LED matrix, there is an always on buzz when the LED is set to maximum intensity so I implemented a Night Mode. Another thing is that a 10K potentiometer is way too much for a piezo buzzer (1K should do the trick).
  I wanted to add a switch to turn both sensors off just to prevent accidental triggering so I used the second button. 
  The code could be optimised by using AVR code instead of digitalReads/Writes for speed and memory. 
https://ocw.cs.pub.ro/courses/pm/prj2021/dbrigalda/502
