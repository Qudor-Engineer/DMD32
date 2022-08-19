/*--------------------------------------------------------------------------------------

 dmd_test.cpp 
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
#include <DMD32.h>        //include Library

/*           P10 pins              */
#define PIN_DMD_nOE 22          //22       //4    // D22 active low Output Enable, setting this low lights all the LEDs in the selected rows. Can pwm it at very high frequency for brightness control.
#define PIN_DMD_A 19            //19       //13   // D19  default is 15   
#define PIN_DMD_B 21            //21       //12   // D21
#define PIN_DMD_CLK 18          //18       //18   // D18_SCK  is SPI Clock if SPI is used   R39
#define PIN_DMD_SCLK  2         //2        //14   // D02
#define PIN_DMD_R_DATA  23      //23       //23   // D23_MOSI is SPI Master Out if SPI is used


#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"
#include "fonts/Arial_38b.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"
#include "fonts/Arial_Black21.h"
#include "fonts/Arial14.h"
#include "fonts/BodoniMTBlack24.h"
#include "fonts/Comic24.h"
#include "fonts/Droid_Sans_24.h"
#include "fonts/Tahoma_32.h"

//To use multiple Display connect displays in Series.
//If you have 8 module and want to make 4*4 display connect all of them in series. 
//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1         //4
#define DISPLAYS_DOWN 1           //2
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

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
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{

// return the clock speed of the CPU
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

}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   byte b;
   
   // 10 x 14 font clock, including demo of OR and NOR modes for pixels so that the flashing colon can be overlayed
   dmd.clearScreen( true );
   dmd.selectFont(Arial_Black_16);
   dmd.drawChar(  0,  3, '2', GRAPHICS_NORMAL );
   dmd.drawChar(  7,  3, '3', GRAPHICS_NORMAL );
   dmd.drawChar( 17,  3, '4', GRAPHICS_NORMAL );
   dmd.drawChar( 25,  3, '5', GRAPHICS_NORMAL );
   dmd.drawChar( 15,  3, ':', GRAPHICS_OR     );   // clock colon overlay on
   delay( 1000 );
   dmd.drawChar( 15,  3, ':', GRAPHICS_NOR    );   // clock colon overlay off
   delay( 1000 );
   dmd.drawChar( 15,  3, ':', GRAPHICS_OR     );   // clock colon overlay on
   delay( 1000 );
   dmd.drawChar( 15,  3, ':', GRAPHICS_NOR    );   // clock colon overlay off
   delay( 1000 );
   dmd.drawChar( 15,  3, ':', GRAPHICS_OR     );   // clock colon overlay on
   delay( 1000 );

   dmd.drawMarquee("Scrolling Text",14,(32*DISPLAYS_ACROSS)-1,0);
   long start=millis();
   long timer=start;
   boolean ret=false;
   while(!ret){
     if ((timer+30) < millis()) {
       ret=dmd.stepMarquee(-1,0);
       timer=millis();
     }
   }
   // half the pixels on
   dmd.drawTestPattern( PATTERN_ALT_0 );
   delay( 1000 );

   // the other half on
   dmd.drawTestPattern( PATTERN_ALT_1 );
   delay( 1000 );
   
   // display some text
   dmd.clearScreen( true );
   dmd.selectFont(System5x7);
   for (byte x=0;x<DISPLAYS_ACROSS;x++) {
     for (byte y=0;y<DISPLAYS_DOWN;y++) {
       dmd.drawString(  2+(32*x),  1+(16*y), "freet", 5, GRAPHICS_NORMAL );
       dmd.drawString(  2+(32*x),  9+(16*y), "ronic", 5, GRAPHICS_NORMAL );
     }
   }
   delay( 2000 );
   
   // draw a border rectangle around the outside of the display
   dmd.clearScreen( true );
   dmd.drawBox(  0,  0, (32*DISPLAYS_ACROSS)-1, (16*DISPLAYS_DOWN)-1, GRAPHICS_NORMAL );
   delay( 1000 );
   
   for (byte y=0;y<DISPLAYS_DOWN;y++) {
     for (byte x=0;x<DISPLAYS_ACROSS;x++) {
       // draw an X
       int ix=32*x;
       int iy=16*y;
       dmd.drawLine(  0+ix,  0+iy, 11+ix, 15+iy, GRAPHICS_NORMAL );
       dmd.drawLine(  0+ix, 15+iy, 11+ix,  0+iy, GRAPHICS_NORMAL );
       delay( 1000 );
   
       // draw a circle
       dmd.drawCircle( 16+ix,  8+iy,  5, GRAPHICS_NORMAL );
       delay( 1000 );
   
       // draw a filled box
       dmd.drawFilledBox( 24+ix, 3+iy, 29+ix, 13+iy, GRAPHICS_NORMAL );
       delay( 1000 );
     }
   }

   // stripe chaser
   for( b = 0 ; b < 20 ; b++ )
   {
      dmd.drawTestPattern( (b&1)+PATTERN_STRIPE_0 );
      delay( 200 );      
   }
   delay( 200 );      
   
}
