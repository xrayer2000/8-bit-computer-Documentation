#include <LiquidCrystal.h>
/* This sketch includes both code for the LCD to work, and also code for the "bootloader"

*/
// data to send to 8 bit computer. Maximum of bytes to send to RAM is 255
byte data[] = { 0x0f, 0x0a, 0x17, 0x0a, 0x1f, 0x00, 0xe2, 0x38, 0x0b, 0x03, 0x2d, 0xcf, 0x2f, 0x06};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte a0;
byte a1;
byte a2;
byte a3;
byte a4;
byte a5;
byte a6;
byte a7;


//Pin connected to SH_CP of 74HC595
int clockPin = 10;
////Pin connected to DS of 74HC595
int dataPin = 9;
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin used to request a new byte from Arduino to the 74HC595
int requestPin = 7;
//Pin used for reseting the index that controls which byte is to be written to the 74HC595
int reset_Pin = 13;
//Pin used for disabling the RAM OE pin so the 74HC595 can feed its data to the EEPROM
int disableRAM_OE_Pin = 6;

int lastRequestState = 0;
int lastResetState = 0;

bool resetState = false;
bool requestState = false;
bool disableRAM_OE = false;

byte byteRead1;
byte byteRead2;
byte completeByte;
byte b1 = 0;
byte b2 = 0;

int index = 0;
int i;

void setup() {
  pinMode(reset_Pin, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(requestPin, INPUT);
  pinMode(disableRAM_OE_Pin, OUTPUT);

  int i = 0;
  Serial.begin(57600); //this baudrate must be the same in the Proccesing sketch

  lcd.begin(16, 2);
  //set pins to output so you can control the shift register
}

void loop() {

  if (Serial.available() > 0)
  {
    byteRead1 = Serial.read();
    delay(1);          //delay(1)
    byteRead2 = Serial.read();
    //Serial.write(byteRead1);
    //Serial.write(byteRead2);
    

    if (byteRead1 >= 65 && byteRead1 <= 70)
      byteRead1 -= 55;
    else if (byteRead1 >= 97 && byteRead1 <= 102)
      byteRead1 -= 87;
    if (byteRead1 >= 48 && byteRead1 <= 57)
      byteRead1 -= 48;

    byteRead1 *= 16;

    if (byteRead2 >= 65 && byteRead2 <= 70)
      byteRead2 -= 55;
    else if (byteRead2 >= 97 && byteRead2 <= 102)
      byteRead2 -= 87;
    if (byteRead2 >= 48 && byteRead2 <= 57)
      byteRead2 -= 48;

    completeByte = byteRead1 + byteRead2;
    data[index] = completeByte;

    Serial.write(completeByte);
    index++;
  }
  else if (Serial.available() == 0){
    
    a0 = highOrLow(0, 0b10000000);
    a1 = highOrLow(1, 0b1000000);
    a2 = highOrLow(2, 0b100000);
    a3 = highOrLow(3, 0b10000);
    a4 = highOrLow(4, 0b1000);
    a5 = highOrLow(5, 0b100);
    a6 = highOrLow(6, 0b10);
    a7 = highOrLow(7, 0b1);
    
    b1 = a0 | a1 | a2 | a3 | a4 | a5 | a6 | a7;
    
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(0);
    lcd.setCursor(1, 0);
    lcd.print(0);
    lcd.setCursor(2, 0);
    lcd.print(0);

    if (b1 < 10)
      lcd.setCursor(2, 0);
    else if (b1 < 100)
      lcd.setCursor(1, 0);
    else if (b1 < 1000)
      lcd.setCursor(0, 0);

    lcd.print(b1);
    if(b1 != b2)
    {
      Serial.write(b1);        // använda denna om jag vill se värderna i processing    ////  ta bort det här om du vill se i processing rätt bytes
      //Serial.println(b1);   // använda denna om jag vill se värderna i "seriell monitor" i Arduino
    }
    b2 = b1;
    
    if (!disableRAM_OE)
    {
      delay(5);
    }
     
    //Serial.println(disableRAM_OE);
    //Serial.println(digitalRead(reset_Pin));
    //Serial.println(resetState);
  }

  requestState = digitalRead(requestPin);
  resetState = digitalRead(reset_Pin);

  if (requestState != lastRequestState && i < sizeof(data))
  {
    if (requestState == LOW)
    {
      //Serial.println(digitalRead(requestPin));
      disableRAM_OE = true;
      digitalWrite(disableRAM_OE_Pin, disableRAM_OE);
      // take the latchPin low so
      // the LEDs don't change while you're sending in bits:
      digitalWrite(latchPin, LOW);
      // shift out the bits:
      //Serial.println("-------------");
      //Serial.println(data[i], HEX);
      shiftOut(dataPin, clockPin, MSBFIRST, data[i]);
      i++;

      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
    }
    else
    {

    }
  }

  lastRequestState = requestState;

  if (i >= sizeof(data) - 1)
  {
    disableRAM_OE = false;
    delay(1);
    digitalWrite(disableRAM_OE_Pin, disableRAM_OE);
  }

  if (resetState != lastResetState)
  {
    if (resetState == HIGH)
    {
      i = 0;
    }
  }

  lastResetState = resetState;

}

byte highOrLow(int pin, byte value)
{
  if (analogRead(pin) > 500)
    return value;
  else
    return 0b00000000;
}
