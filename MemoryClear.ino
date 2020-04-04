//Christian Leonard
//March 9, 2020
//HN400

//This program is to be run to 'reset' Walter's memory after each new maze solving attempt

#include <EEPROM.h>

void setup() 
{
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    EEPROM.update(i, 0); //using update preserves the lifespan of the EEPROM memory on the Arduino Board, only changing the values as necessary
  }
}

void loop() 
{
  /** Empty loop. **/
}
