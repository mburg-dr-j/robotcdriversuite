#pragma config(Sensor, S1,     MSLSA,          sensorI2CCustomFastSkipStates9V)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * $Id: mindsensors-ligthsensorarray-test2.c $
 */

/**
 * mindsensors-lineleader.h provides an API for the Mindsensors LineLeader Sensor.  This program
 * demonstrates how to use that API.
 *
 * Changelog:
 * - 0.1: Initial release
 * - 0.2: More comments
 *
 * Credits:
 * - Big thanks to Mindsensors for providing me with the hardware necessary to write and test this.
 *
 * License: You may use this code as you wish, provided you give credit where it's due.
 *
 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 3.60 AND HIGHER.

 * Xander Soldaat (xander_at_botbench.com)
 * 28 November 2009
 * version 0.2
 */

#include "mindsensors-lightsensorarray.h"

task main {
  tIntArray signalstr;

  while (true) {
    // Fetch all of the uncalibrated sensor values and display them
    if (!MSLSAreadRawSensors(MSLSA, &signalstr[0]))
    {
      eraseDisplay();
      PlaySound(soundException);
      nxtDisplayCenteredTextLine(4, "error!");
      wait1Msec(5000);
    }
    for (int i = 0; i < 8; i++) {
      nxtDisplayTextLine(i, "S[%d]: %d", i, signalstr[i]);
    }
    wait1Msec(50);
  }
}

/*
 * $Id: mindsensors-ligthsensorarray-test2.c $
 */
