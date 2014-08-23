/*
  Minimal version of the DCF77 library by Thijs Elenbaas.
  
  Original: 
  Copyright (c) Thijs Elenbaas 2012
  http://playground.arduino.cc/Code/DCF77
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Changed by Torsten Braun to be minimalistic for this project.
*/

#include "DCF77.h"

int                DCF77::DCFPin              = 0;
int                DCF77::DCFInterrupt        = 0;
int                DCF77::previousLeadingEdge = 0;
int                DCF77::leadingEdge         = 0;
int                DCF77::trailingEdge        = 0;
bool               DCF77::up                  = false;
int                DCF77::bufferPosition      = 0;
unsigned long long DCF77::runningBuffer       = 0;
unsigned long long DCF77::processingBuffer    = 0;

volatile unsigned long long DCF77::filledBuffer          = 0;
volatile bool               DCF77::filledBufferAvailable = false;

DCF77::ParityFlags DCF77::flags = {0,0,0,0};

DCF77::DCF77(int _DCFPin, int _DCFInterrupt)
{
  DCFPin       = _DCFPin;
  DCFInterrupt = _DCFInterrupt;
}

void DCF77::start()
{
  attachInterrupt(DCFInterrupt, interruptHandler, CHANGE);
}

void DCF77::stop()
{
  detachInterrupt(DCFInterrupt);
}

void DCF77::interruptHandler()
{  
  int  difference;
  int  flankTime = millis();
  byte value     = digitalRead(DCFPin);
  
  // flank detected too quickly after the previous flank
  if ((flankTime - previousLeadingEdge) < DCF_REJECTION_TIME)
    return;
  
  // pulse is too short
  if ((flankTime - leadingEdge) < DCF_REJECT_PULSE_WIDTH)
    return;
  
  if (HIGH == value)
  {
    if (!up)
    {
      leadingEdge = flankTime;
      up          = true;
    }
  }
  else
  {
    if (up)
    {
      trailingEdge = flankTime;
      difference   = trailingEdge - leadingEdge;
      
      if ((leadingEdge - previousLeadingEdge) > DCF_SYNC_TIME)
        finalizeBuffer();
        
      previousLeadingEdge = leadingEdge;
      
      if (difference < DCF_SPLIT_TIME)
        appendSignal(0);
      else
        appendSignal(1);
        
      up = false;
    }
  }
}

inline void DCF77::appendSignal(unsigned char signal)
{
  runningBuffer |= ((unsigned long long) signal << bufferPosition);
  
  bufferPosition++;
  
  if (bufferPosition > 59)
    finalizeBuffer();
}

inline void DCF77::finalizeBuffer()
{  
  if (59 == bufferPosition)
  {
    filledBuffer = runningBuffer;
    
    initBuffer();
    
    filledBufferAvailable = true;   
  }
  else
  {
    initBuffer();
  }
}

inline void DCF77::initBuffer()
{
  runningBuffer  = 0;
  bufferPosition = 0;
}

bool DCF77::hasNewTime()
{
  return filledBufferAvailable;
}

bool DCF77::getTime(int* time)
{
  processingBuffer = filledBuffer;
  filledBufferAvailable = false;
  
  calculateBufferParities();
  
  struct DCF77Buffer *rx_buffer;
  rx_buffer = (struct DCF77Buffer *)(unsigned long long)&processingBuffer;
  
  if (flags.parityMin == rx_buffer->P1 && flags.parityHour == rx_buffer->P2)
  {
    time[0] = rx_buffer->Min  - ((rx_buffer->Min / 16)  * 6);
    time[1] = rx_buffer->Hour - ((rx_buffer->Hour / 16) * 6);
    
    return true;
  }
  
  return false;
}

void DCF77::calculateBufferParities() {	
  flags.parityFlag = 0;	
    
  for(int pos = 0; pos < 59; pos++) 
  {
    bool s = (processingBuffer >> pos) & 1;  

    if (21 == pos || 29 == pos || 36 == pos) 
      flags.parityFlag = 0;

    if (28 == pos) 
      flags.parityMin = flags.parityFlag;

    if (35 == pos)
      flags.parityHour = flags.parityFlag;

    if (58 == pos)
      flags.parityDate = flags.parityFlag;
      
    if (1 == s)
      flags.parityFlag = flags.parityFlag ^ 1;
  }
}
