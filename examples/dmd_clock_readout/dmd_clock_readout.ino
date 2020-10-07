/*--------------------------------------------------------------------------------------

 dmd_clock_readout.cpp 
   Demo and example project for the Freetronics DMD, a 512 LED matrix display
   panel arranged in a 32 x 16 layout.

This LIibrary (DMD32) and example are  fork of original DMD library  was modified to work on ESP32 
Modified by: Khudhur Alfarhan  // Qudoren@gmail.com
1/Oct./2020

 See http://www.freetronics.com/dmd for resources and a getting started guide.

 Note that the DMD32 library uses the VSPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.

 USAGE NOTES
 -----------
 - Place the DMD library folder into the "arduino/libraries/" folder of your Arduino installation.
 - Restart the IDE.
 - In the Arduino IDE, go to Tools > Board > and choose any ESP32 board
 - In the Arduino IDE, you can open File > Examples > DMD > dmd_demo, or dmd_clock_readout, and get it
   running straight away!

 * See the documentation on Github or attached images to find the pins that should be connected to the DMD LED display
  

 This example code is in the public domain.
 The DMD32 library is open source (GPL), for more see DMD32.cpp and DMD32.h

--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/

#include <DMD32.h>        //
#include "fonts/Arial_black_16.h"

//Fire up the DMD library as dmd
DMD dmd(1,1);

   //Timer setup
  //create a hardware timer  of ESP32
  hw_timer_t * timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
  dmd.scanDisplayBySPI();
}

/*--------------------------------------------------------------------------------------
  Show clock numerals on the screen from a 4 digit time value, and select whether the
  flashing colon is on or off
--------------------------------------------------------------------------------------*/
void ShowClockNumbers( unsigned int uiTime, byte bColonOn )
{
   dmd.clearScreen(true);
   dmd.drawChar(  1,  3,'0'+((uiTime%10000)/1000), GRAPHICS_NORMAL );   // thousands
   dmd.drawChar(  8,  3, '0'+((uiTime%1000) /100),  GRAPHICS_NORMAL );   // hundreds
   dmd.drawChar( 17,  3, '0'+((uiTime%100)  /10),   GRAPHICS_NORMAL );   // tens
   dmd.drawChar( 25,  3, '0'+ (uiTime%10),          GRAPHICS_NORMAL );   // units
   if( bColonOn )
      dmd.drawChar( 15,  3, ':', GRAPHICS_OR     );   // clock colon overlay on
   else
      dmd.drawChar( 15,  3, ':', GRAPHICS_NOR    );   // clock colon overlay off
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
   
    //return the clock speed of the CPU
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  
  // Use 1st timer of 4 
  // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
  timer = timerBegin(0, cpuClock, true);
  // Attach triggerScan function to our timer 
  timerAttachInterrupt(timer, &triggerScan, true);
  // Set alarm to call triggerScan function  
  // Repeat the alarm (third parameter) 
  timerAlarmWrite(timer, 300, true);

  // Start an alarm 
  timerAlarmEnable(timer);

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(Arial_Black_16);

}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   unsigned int ui;
   
   // 10 x 14 font clock, including demo of OR and NOR modes for pixels so that the flashing colon can be overlayed
   ui = 1234;
   ShowClockNumbers( ui, true );
   delay( 1000 );
   ShowClockNumbers( ui, false );
   delay( 1000 );
   ShowClockNumbers( ui, true );
   delay( 1000 );
   ShowClockNumbers( ui, false );
   delay( 1000 );

   ui = 2345;
   ShowClockNumbers( ui, true );
   delay( 1000 );
   ShowClockNumbers( ui, false );
   delay( 1000 );
   ShowClockNumbers( ui, true );
   delay( 1000 );
   ShowClockNumbers( ui, false );
   delay( 1000 );
}
