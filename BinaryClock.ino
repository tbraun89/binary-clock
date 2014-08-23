/*
  Copyright (C) 2014 Torsten Braun

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "DateTime.h"
#include "DCF77.h"

#define DCF_PIN       2
#define DCF_INTERRUPT INT1

#define UPDATE_LEDS_DELAY 1000

// Minute Pins
#define MIN_1   12
#define MIN_2   11
#define MIN_4   10
#define MIN_8   9
#define MIN_16  8
#define MIN_32  7
// Hour Pins
#define HOUR_1  6
#define HOUR_2  5
#define HOUR_4  4
#define HOUR_8  3 
#define HOUR_16 13

DCF77 DCF      = DCF77(DCF_PIN, DCF_INTERRUPT);
bool  needSync = true;

void setup() 
{
  Serial.begin(115200);

  initLedPins();
  DCF.start();
}


void loop() 
{
  int DCFtimeBuffer[2] = { 0 };
  
  if (needSync)
  {
    if (DCF.hasNewTime()) 
    {
      if (DCF.getTime(DCFtimeBuffer))
      {
        DateTime.sync(DateTime.makeTime(0, DCFtimeBuffer[0], DCFtimeBuffer[1], 0, 0, 0));
        
        if (DateTime.available())
        {
          needSync = false;
          DCF.stop();
        }
      }
    }
  }

  if (DateTime.available())
  {
    writeMinute(DateTime.Minute);
    writeHour(DateTime.Hour);
    
    if (2 == DateTime.Hour)
    {
      needSync = true;
      DCF.start();
    }
  }
    
  delay(UPDATE_LEDS_DELAY);
}

void writeMinute(int val) {
  digitalWrite(MIN_1,  ((val & ( 1 << 0)) >> 0) ? LOW : HIGH);
  digitalWrite(MIN_2,  ((val & ( 1 << 1)) >> 1) ? LOW : HIGH);
  digitalWrite(MIN_4,  ((val & ( 1 << 2)) >> 2) ? LOW : HIGH);
  digitalWrite(MIN_8,  ((val & ( 1 << 3)) >> 3) ? LOW : HIGH);
  digitalWrite(MIN_16, ((val & ( 1 << 4)) >> 4) ? LOW : HIGH);
  digitalWrite(MIN_32, ((val & ( 1 << 5)) >> 5) ? LOW : HIGH);
}

void writeHour(int val) {
  digitalWrite(HOUR_1,  ((val & ( 1 << 0)) >> 0) ? LOW : HIGH);
  digitalWrite(HOUR_2,  ((val & ( 1 << 1)) >> 1) ? LOW : HIGH);
  digitalWrite(HOUR_4,  ((val & ( 1 << 2)) >> 2) ? LOW : HIGH);
  digitalWrite(HOUR_8,  ((val & ( 1 << 3)) >> 3) ? LOW : HIGH);
  digitalWrite(HOUR_16, ((val & ( 1 << 4)) >> 4) ? LOW : HIGH);
}

void initLedPins() {
  pinMode(MIN_1,   OUTPUT);
  pinMode(MIN_2,   OUTPUT); 
  pinMode(MIN_4,   OUTPUT); 
  pinMode(MIN_8,   OUTPUT); 
  pinMode(MIN_16,  OUTPUT); 
  pinMode(MIN_32,  OUTPUT);
  pinMode(HOUR_1,  OUTPUT);
  pinMode(HOUR_2,  OUTPUT);
  pinMode(HOUR_4,  OUTPUT);
  pinMode(HOUR_8,  OUTPUT);
  pinMode(HOUR_16, OUTPUT);
  
  digitalWrite(MIN_1,   HIGH);
  digitalWrite(MIN_2,   HIGH);
  digitalWrite(MIN_4,   HIGH);
  digitalWrite(MIN_8,   HIGH);
  digitalWrite(MIN_16,  HIGH);
  digitalWrite(MIN_32,  HIGH);
  digitalWrite(HOUR_1,  HIGH);
  digitalWrite(HOUR_2,  HIGH);
  digitalWrite(HOUR_4,  HIGH);
  digitalWrite(HOUR_8,  HIGH);
  digitalWrite(HOUR_16, HIGH);
}
