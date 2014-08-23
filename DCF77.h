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

#ifndef DCF77_h
#define DCF77_h

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

#define DCF_REJECTION_TIME 700     // Pulse-to-Pulse rejection time. 
#define DCF_REJECT_PULSE_WIDTH 50  // Minimal pulse width
#define DCF_SPLIT_TIME 180         // Specifications distinguishes pulse width 100 ms and 200 ms. In practice we see 130 ms and 230
#define DCF_SYNC_TIME 1500         // Specifications defines 2000 ms pulse for end of sequence

class DCF77 
{
private:
  static int                DCFPin;
  static int                DCFInterrupt;
  static int                previousLeadingEdge;
  static int                leadingEdge;
  static int                trailingEdge;
  static bool               up;
  static int                bufferPosition;
  static unsigned long long runningBuffer;
  static unsigned long long processingBuffer;
  
  static volatile bool               filledBufferAvailable;
  static volatile unsigned long long filledBuffer;
  
  struct DCF77Buffer {
    //unsigned long long prefix	:21;
    unsigned long long prefix   :17;
    unsigned long long CEST     :1; // CEST 
    unsigned long long CET      :1; // CET 
    unsigned long long unused   :2; // unused bits
    unsigned long long Min      :7; // minutes
    unsigned long long P1       :1; // parity minutes
    unsigned long long Hour     :6; // hours
    unsigned long long P2       :1; // parity hours
    unsigned long long Day      :6; // day
    unsigned long long Weekday  :3; // day of week
    unsigned long long Month    :5; // month
    unsigned long long Year     :8; // year (5 -> 2005)
    unsigned long long P3       :1; // parity
  };
  
  struct ParityFlags{
    unsigned char parityFlag :1;
    unsigned char parityMin  :1;
    unsigned char parityHour :1;
    unsigned char parityDate :1;
  } static flags;
  
protected:
  static void interruptHandler();
  static void appendSignal(unsigned char signal);
  static void finalizeBuffer();
  static void initBuffer();
  static void calculateBufferParities();

public:
  DCF77(int _DCFPin, int _DCFInterrupt);

  void start();
  void stop();
  bool hasNewTime();
  bool getTime(int* time);
};

#endif

