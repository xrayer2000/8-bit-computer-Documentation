#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

byte data[8];
int sizeofEEPPROM = 255; //8191 

void setup() {

  
  data[0] = 0xff;
  data[1] = 0x23;
  data[2] = 0xc3;
  data[3] = 0x90;
  data[4] = 0x02;
  data[5] = 0x01;
  data[6] = 0x03;
  data[7] = 0x04;

  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);

  Serial.begin(57600);

  Serial.println("Erease or program? 'e' or 'p'");
  
  while (Serial.available() <= 0);
  char answere = Serial.read();
  Serial.println(answere);
  if (answere == 'e')
  {
    Serial.println("Ereasing");
    for (int address = 0; address <= sizeofEEPPROM; address += 1)
      writeEEPROM(address, 0x00);
  }
  else if(answere == 'p')
  {
      Serial.println("Programing");
    for (int address = 0; address < sizeof(data); address += 1)
      writeEEPROM(address, data[address]);
  
  }
 

    Serial.println(" done");
  Serial.println("Reading EEPROM");
  printContents();
  
  
}

void setAddress(int address, bool outputEnable) {
  
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) );
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address | (outputEnable ? 0x00 : 0x01));

  digitalWrite(SHIFT_LATCH, LOW);
  delayMicroseconds(10);
  digitalWrite(SHIFT_LATCH, HIGH);

}

byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(10);
  digitalWrite(WRITE_EN, HIGH);
  delay(1);
}

void printContents() {
  for (int base = 0; base <= sizeofEEPPROM; base += 16)
  {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}



void loop() {
  // put your main code here, to run repeatedly:

}
