#pragma config(Sensor, S4,     NXT2WIFI,       sensorHighSpeed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * $Id: benedettelli-nxt2wifi-test2.c $
 */

/**
 * benedettelli-nxt2wifi.h provides an API for Daniele's NXT2WiFi Sensor. This program
 * demonstrates how to use that API.  This program is opens a connection to a BOFH server
 * and fetches an excuse and displays it on the screen.  It is a demo for opening a TCP socket
 * to a remote host.
 *
 * Changelog:
 * - 0.1: Initial release
 *
 * Credits:
 * - Big thanks to Daniele Benedettelli for providing me with the hardware necessary to write and test this.
 *
 * License: You may use this code as you wish, provided you give credit where it's due.
 *
 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 3.60 AND HIGHER.

 * Xander Soldaat (xander_at_botbench.com)
 * 22 July 2012
 * version 0.1
 */

#include "common.h"
#include "benedettelli-nxt2wifi.h"

long txbytes = 0;
long rxbytes = 0;
string IPaddress = "0.0.0.0";
string connStatus = "disconnected";
string dataStrings[4];
tHugeByteArray data;
int sizeOfReceivedData;

task updateScreen()
{
  while(true)
  {
    nxtDisplayTextLine(0, "Stat: %s", connStatus);
    nxtDisplayTextLine(1, "%s",IPaddress);
    nxtDisplayTextLine(2, "-------------------");
    nxtDisplayTextLine(3, "%s", dataStrings[0]);
		nxtDisplayTextLine(4, "%s", dataStrings[1]);
		nxtDisplayTextLine(5, "%s", dataStrings[2]);
		nxtDisplayTextLine(6, "%s", dataStrings[3]);
    nxtDisplayTextLine(7, "RX/TX: %d/%d", rxbytes, txbytes);
    wait1Msec(100);
  }
}

void processData()
{
  // Used for going through the current line so we
  // can print it
  string line = "";
  string singleword = "";

	// What line are we current printing on?
  int screenLine = 0;

  dataStrings[0] = "";
  dataStrings[1] = "";
  dataStrings[2] = "";
  dataStrings[3] = "";
  for (int i = 0; i < sizeOfReceivedData; i++)
  {
    // It's a printable character
    if (RS485rxbuffer[i] > 0x20)
    {
      // Append to the current word
      strncat(singleword, &RS485rxbuffer[i], 1);
    }
    // we've reached a word or line boundary
    else if ((RS485rxbuffer[i] == 0x20) || (RS485rxbuffer[i] == 0x0A))
    {
      // is the current word and the current line more than we can print?
      if ((strlen(line) + strlen(singleword)) > 16)
      {
        // print out the current line
        dataStrings[screenLine++] = line;
        // current word becomes new line
        memcpy(line, singleword, sizeof(singleword));
        // blank out the current word
        singleword = "";
      }
      else
      {
        // Append a space to the current line before sticking
        // the new word to it
        if (strlen(line) > 0)
          strcat(line, " ");
        strcat(line, singleword);
        singleword = "";
      }

      // Are we at the end of the line and need to print it out regardless?
      if (RS485rxbuffer[i] == 0x0A)
      {
        dataStrings[screenLine++] = line;
        // Reset the current line
        line = "";
      }
    }
  }
}
task main ()
{
  string BOFHserver = "192.168.0.100"; // Linux VM
  int BOFHport = 6666;
  string dataString;
  getFriendlyName(dataString);

  int avail = 0;

  nNxtButtonTask = -2;
  StartTask(updateScreen);

  // initialise the port, etc
  RS485initLib();

  memset(RS485rxbuffer, 0, sizeof(RS485rxbuffer));
  memset(RS485txbuffer, 0, sizeof(RS485txbuffer));

  // Disconnect if already connected
  N2WDisconnect();
  N2WchillOut();

  if (!N2WCustomExist())
  {
    StopTask(updateScreen);
    wait1Msec(50);
    eraseDisplay();
    PlaySound(soundException);
    nxtDisplayCenteredBigTextLine(1, "ERROR");
    nxtDisplayTextLine(3, "No custom profile");
    nxtDisplayTextLine(4, "configured!!");
    while(true) EndTimeSlice();
  }

  N2WLoad();

  wait1Msec(100);
  N2WConnect(true);
  connStatus = "connecting";

  while (!N2WConnected())
    wait1Msec(1000);

  connStatus = "connected";
  PlaySound(soundBeepBeep);

  wait1Msec(3000);
  N2WgetIP(IPaddress);

  wait1Msec(1000);


  while (true)
  {
    while (nNxtButtonPressed != kEnterButton) EndTimeSlice();
    while (nNxtButtonPressed != kNoButton) EndTimeSlice();
    if (N2WTCPOpenClient(1, BOFHserver, BOFHport)) {
      data[0] = 0x0A;
      N2WTCPWrite(1, data, 1);
      txbytes++;
      // How many bytes are available in the buffers?
	    avail = N2WTCPAvail(1);
	    if (avail > 0)
	    {
	      sizeOfReceivedData = avail;
	      rxbytes += avail;
	      N2WchillOut();
	      // read the current buffer
	      N2WTCPRead(1, avail);
	      processData();
	      for (int i = 0; i < avail; i++)
	      {
	        writeDebugStream("%c", RS485rxbuffer[i]);
	      }
	      writeDebugStreamLine("");
	    }
	    // Wait a bit
      N2WchillOut();
      // Disconnect
      N2WTCPClose(1);
    }
  }
}
