#define FLASH_A00 12
#define FLASH_A01 11
#define FLASH_A02 10
#define FLASH_A03 9
#define FLASH_A04 8
#define FLASH_A05 7
#define FLASH_A06 6
#define FLASH_A07 5
#define FLASH_A08 4
#define FLASH_A09 3
#define FLASH_A10 2
#define FLASH_A11 14
#define FLASH_A12 15
#define FLASH_A13 16
#define FLASH_A14 17
#define FLASH_A15 18
#define FLASH_A16 19
#define FLASH_A17 20
#define FLASH_A18 21
#define FLASH_A19 23
#define FLASH_A20 22
#define FLASH_A21 25
#define FLASH_A22 24

#define FLASH_DQ00 27
#define FLASH_DQ01 26
#define FLASH_DQ02 29
#define FLASH_DQ03 28
#define FLASH_DQ04 31
#define FLASH_DQ05 30
#define FLASH_DQ06 33
#define FLASH_DQ07 32
#define FLASH_DQ08 35
#define FLASH_DQ09 34
#define FLASH_DQ10 37
#define FLASH_DQ11 36
#define FLASH_DQ12 39
#define FLASH_DQ13 38
#define FLASH_DQ14 41
#define FLASH_DQ15 40

#define FLASH_OE_n 43
#define FLASH_IC1_CE_n 42
#define FLASH_IC2_CE_n 45
#define FLASH_WP_n 44
#define FLASH_RST_n 47
#define FLASH_RY_BY 46
/*
#define EEPROM_CLK 49
#define EEPROM_DI 48
#define EEPROM_CS 51
#define EEPROM_DO 50
*/
#define BUTTON01 53
#define BUTTON02 52
#define BUTTON03 A0
#define IC1_WE_n A1
#define IC2_WE_n A2

#define BAUD_RATE 115200
//#define BAUD_RATE 57600

#define FLASH_PROGMEM
#define FLASH_READ_DWORD(x) (*(uint32_t *)(x))

#define BLOCK_SIZE 512

const uint32_t INITIAL_VALUE = ~0L;

static const uint32_t crc32_table[] FLASH_PROGMEM = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
  0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
  0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

// ---------------------------------------------------------------------------------------------------------

uint32_t updateCRC32(uint8_t data, uint32_t _state) {
  uint8_t tbl_idx = 0;

  tbl_idx = _state ^ (data >> (0 * 4));
  _state = FLASH_READ_DWORD(crc32_table + (tbl_idx & 0x0f)) ^ (_state >> 4);
  tbl_idx = _state ^ (data >> (1 * 4));
  _state = FLASH_READ_DWORD(crc32_table + (tbl_idx & 0x0f)) ^ (_state >> 4);

  return _state;
}

// ---------------------------------------------------------------------------------------------------------

int display_progress_bar(unsigned int current, unsigned int total, int last_value) {
  // Calculate the percentage completed
  int percentage = (int)(((float)current / (float)total) * 100);
  // Determine the size of the progress bar
  int size = 50;  // Number of characters in the progress bar

  // Calculate how many characters should be filled
  int filled = (percentage * size) / 100;

  // Build the progress bar string
  String progressBar = "[";
  for (int i = 0; i < filled; i++) {
    progressBar += "=";
  }
  for (int i = filled; i < size; i++) {
    progressBar += " ";
  }
  progressBar += "] ";

  // Print the progress bar and percentage
  if ((last_value == -1) || (last_value != percentage)) {
    SerialUSB.print(progressBar);
    SerialUSB.print(percentage);
    SerialUSB.println("%");
  }

  return percentage;
}

// ---------------------------------------------------------------------------------------------------------

String pad_str(uint32_t sourceStr, int padWidth) {
  String inputStr = String(sourceStr, HEX);
  inputStr.toUpperCase();

  while (inputStr.length() < padWidth) {
    inputStr = "0" + inputStr;
  }

  return inputStr;
}

// ---------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_A00, OUTPUT);
  pinMode(FLASH_A01, OUTPUT);
  pinMode(FLASH_A02, OUTPUT);
  pinMode(FLASH_A03, OUTPUT);
  pinMode(FLASH_A04, OUTPUT);
  pinMode(FLASH_A05, OUTPUT);
  pinMode(FLASH_A06, OUTPUT);
  pinMode(FLASH_A07, OUTPUT);
  pinMode(FLASH_A08, OUTPUT);
  pinMode(FLASH_A09, OUTPUT);
  pinMode(FLASH_A10, OUTPUT);
  pinMode(FLASH_A11, OUTPUT);
  pinMode(FLASH_A12, OUTPUT);
  pinMode(FLASH_A13, OUTPUT);
  pinMode(FLASH_A14, OUTPUT);
  pinMode(FLASH_A15, OUTPUT);
  pinMode(FLASH_A16, OUTPUT);
  pinMode(FLASH_A17, OUTPUT);
  pinMode(FLASH_A18, OUTPUT);
  pinMode(FLASH_A19, OUTPUT);
  pinMode(FLASH_A20, OUTPUT);
  pinMode(FLASH_A21, OUTPUT);
  pinMode(FLASH_A22, OUTPUT);
  init_address_pins();
/*  
  pinMode(FLASH_DQ00, INPUT);pinMode(FLASH_DQ01, INPUT);pinMode(FLASH_DQ02, INPUT);
  pinMode(FLASH_DQ03, INPUT);pinMode(FLASH_DQ04, INPUT);pinMode(FLASH_DQ05, INPUT);
  pinMode(FLASH_DQ06, INPUT);pinMode(FLASH_DQ07, INPUT);pinMode(FLASH_DQ08, INPUT);
  pinMode(FLASH_DQ09, INPUT);pinMode(FLASH_DQ10, INPUT);pinMode(FLASH_DQ11, INPUT);
  pinMode(FLASH_DQ12, INPUT);pinMode(FLASH_DQ13, INPUT);pinMode(FLASH_DQ14, INPUT);
  pinMode(FLASH_DQ15, INPUT);
*/
  pinMode(BUTTON01, INPUT_PULLUP);
  pinMode(BUTTON02, INPUT_PULLUP);
  pinMode(BUTTON03, INPUT_PULLUP);

  pinMode(FLASH_OE_n, OUTPUT);
  pinMode(FLASH_IC1_CE_n, OUTPUT);
  pinMode(FLASH_IC2_CE_n, OUTPUT);
  pinMode(FLASH_WP_n, OUTPUT);
  pinMode(FLASH_RST_n, OUTPUT);
  pinMode(FLASH_RY_BY, INPUT);
  /*
  pinMode(EEPROM_CLK, OUTPUT);
  pinMode(EEPROM_DI, INPUT);
  pinMode(EEPROM_CS, OUTPUT);
  pinMode(EEPROM_DO, OUTPUT);
*/
  pinMode(IC1_WE_n, OUTPUT);
  pinMode(IC2_WE_n, OUTPUT);

  // write protect on initialization
  digitalWrite(IC1_WE_n, HIGH);
  digitalWrite(IC2_WE_n, HIGH);

  // no chip enabled by default
  digitalWrite(FLASH_IC1_CE_n, HIGH);
  digitalWrite(FLASH_IC2_CE_n, HIGH);

  // OE inactive on initialization
  digitalWrite(FLASH_OE_n, HIGH);

  // reset the chip
  digitalWrite(FLASH_RST_n, HIGH);
  delay(10);
  digitalWrite(FLASH_RST_n, LOW);
  delay(10);
  digitalWrite(FLASH_RST_n, HIGH);

  Serial.begin(BAUD_RATE);
  SerialUSB.begin(BAUD_RATE);

  while (!SerialUSB) {
  }
}

// ---------------------------------------------------------------------------------------------------------

void configure_datapins_read() {
  pinMode(FLASH_DQ00, INPUT);
  pinMode(FLASH_DQ01, INPUT);
  pinMode(FLASH_DQ02, INPUT);
  pinMode(FLASH_DQ03, INPUT);
  pinMode(FLASH_DQ04, INPUT);
  pinMode(FLASH_DQ05, INPUT);
  pinMode(FLASH_DQ06, INPUT);
  pinMode(FLASH_DQ07, INPUT);
  pinMode(FLASH_DQ08, INPUT);
  pinMode(FLASH_DQ09, INPUT);
  pinMode(FLASH_DQ10, INPUT);
  pinMode(FLASH_DQ11, INPUT);
  pinMode(FLASH_DQ12, INPUT);
  pinMode(FLASH_DQ13, INPUT);
  pinMode(FLASH_DQ14, INPUT);
  pinMode(FLASH_DQ15, INPUT);
}

// ---------------------------------------------------------------------------------------------------------

void configure_datapins_write() {
  pinMode(FLASH_DQ00, OUTPUT);
  pinMode(FLASH_DQ01, OUTPUT);
  pinMode(FLASH_DQ02, OUTPUT);
  pinMode(FLASH_DQ03, OUTPUT);
  pinMode(FLASH_DQ04, OUTPUT);
  pinMode(FLASH_DQ05, OUTPUT);
  pinMode(FLASH_DQ06, OUTPUT);
  pinMode(FLASH_DQ07, OUTPUT);
  pinMode(FLASH_DQ08, OUTPUT);
  pinMode(FLASH_DQ09, OUTPUT);
  pinMode(FLASH_DQ10, OUTPUT);
  pinMode(FLASH_DQ11, OUTPUT);
  pinMode(FLASH_DQ12, OUTPUT);
  pinMode(FLASH_DQ13, OUTPUT);
  pinMode(FLASH_DQ14, OUTPUT);
  pinMode(FLASH_DQ15, OUTPUT);
}

// ---------------------------------------------------------------------------------------------------------

void init_address_pins() {
  int init_state = LOW;
  digitalWrite(FLASH_A00, init_state);
  digitalWrite(FLASH_A01, init_state);
  digitalWrite(FLASH_A02, init_state);
  digitalWrite(FLASH_A03, init_state);
  digitalWrite(FLASH_A04, init_state);
  digitalWrite(FLASH_A05, init_state);
  digitalWrite(FLASH_A06, init_state);
  digitalWrite(FLASH_A07, init_state);
  digitalWrite(FLASH_A08, init_state);
  digitalWrite(FLASH_A09, init_state);
  digitalWrite(FLASH_A10, init_state);
  digitalWrite(FLASH_A11, init_state);
  digitalWrite(FLASH_A12, init_state);
  digitalWrite(FLASH_A13, init_state);
  digitalWrite(FLASH_A14, init_state);
  digitalWrite(FLASH_A15, init_state);
  digitalWrite(FLASH_A16, init_state);
  digitalWrite(FLASH_A17, init_state);
  digitalWrite(FLASH_A18, init_state);
  digitalWrite(FLASH_A19, init_state);
  digitalWrite(FLASH_A20, init_state);
  digitalWrite(FLASH_A21, init_state);
  digitalWrite(FLASH_A22, init_state);
}

// ---------------------------------------------------------------------------------------------------------

uint16_t read_data(uint32_t address) {
  digitalWrite(FLASH_A00, (address >> 0) & 1);
  digitalWrite(FLASH_A01, (address >> 1) & 1);
  digitalWrite(FLASH_A02, (address >> 2) & 1);
  digitalWrite(FLASH_A03, (address >> 3) & 1);
  digitalWrite(FLASH_A04, (address >> 4) & 1);
  digitalWrite(FLASH_A05, (address >> 5) & 1);
  digitalWrite(FLASH_A06, (address >> 6) & 1);
  digitalWrite(FLASH_A07, (address >> 7) & 1);
  digitalWrite(FLASH_A08, (address >> 8) & 1);
  digitalWrite(FLASH_A09, (address >> 9) & 1);
  digitalWrite(FLASH_A10, (address >> 10) & 1);
  digitalWrite(FLASH_A11, (address >> 11) & 1);
  digitalWrite(FLASH_A12, (address >> 12) & 1);
  digitalWrite(FLASH_A13, (address >> 13) & 1);
  digitalWrite(FLASH_A14, (address >> 14) & 1);
  digitalWrite(FLASH_A15, (address >> 15) & 1);
  digitalWrite(FLASH_A16, (address >> 16) & 1);
  digitalWrite(FLASH_A17, (address >> 17) & 1);
  digitalWrite(FLASH_A18, (address >> 18) & 1);
  digitalWrite(FLASH_A19, (address >> 19) & 1);
  digitalWrite(FLASH_A20, (address >> 20) & 1);
  digitalWrite(FLASH_A21, (address >> 21) & 1);
  digitalWrite(FLASH_A22, (address >> 22) & 1);

  delayMicroseconds(1);
  digitalWrite(FLASH_OE_n, LOW);
  delayMicroseconds(1);

  uint16_t data = 0;

  data |= digitalRead(FLASH_DQ00) << 0;
  data |= digitalRead(FLASH_DQ01) << 1;
  data |= digitalRead(FLASH_DQ02) << 2;
  data |= digitalRead(FLASH_DQ03) << 3;
  data |= digitalRead(FLASH_DQ04) << 4;
  data |= digitalRead(FLASH_DQ05) << 5;
  data |= digitalRead(FLASH_DQ06) << 6;
  data |= digitalRead(FLASH_DQ07) << 7;
  data |= digitalRead(FLASH_DQ08) << 8;
  data |= digitalRead(FLASH_DQ09) << 9;
  data |= digitalRead(FLASH_DQ10) << 10;
  data |= digitalRead(FLASH_DQ11) << 11;
  data |= digitalRead(FLASH_DQ12) << 12;
  data |= digitalRead(FLASH_DQ13) << 13;
  data |= digitalRead(FLASH_DQ14) << 14;
  data |= digitalRead(FLASH_DQ15) << 15;

  digitalWrite(FLASH_OE_n, HIGH);
  delayMicroseconds(1);

  return data;
}

// ---------------------------------------------------------------------------------------------------------

uint16_t read_data_same_address() {
  digitalWrite(FLASH_OE_n, LOW);
  delayMicroseconds(1);

  uint16_t data = 0;

  data |= digitalRead(FLASH_DQ00) << 0;
  data |= digitalRead(FLASH_DQ01) << 1;
  data |= digitalRead(FLASH_DQ02) << 2;
  data |= digitalRead(FLASH_DQ03) << 3;
  data |= digitalRead(FLASH_DQ04) << 4;
  data |= digitalRead(FLASH_DQ05) << 5;
  data |= digitalRead(FLASH_DQ06) << 6;
  data |= digitalRead(FLASH_DQ07) << 7;
  data |= digitalRead(FLASH_DQ08) << 8;
  data |= digitalRead(FLASH_DQ09) << 9;
  data |= digitalRead(FLASH_DQ10) << 10;
  data |= digitalRead(FLASH_DQ11) << 11;
  data |= digitalRead(FLASH_DQ12) << 12;
  data |= digitalRead(FLASH_DQ13) << 13;
  data |= digitalRead(FLASH_DQ14) << 14;
  data |= digitalRead(FLASH_DQ15) << 15;

  digitalWrite(FLASH_OE_n, HIGH);
  delayMicroseconds(1);

  return data;
}

// ---------------------------------------------------------------------------------------------------------

bool read_data_same_address_dq07() {
  digitalWrite(FLASH_OE_n, LOW);
  delayMicroseconds(1);

  bool data = false;

  data = (bool) digitalRead(FLASH_DQ07) << 7;

  digitalWrite(FLASH_OE_n, HIGH);
  delayMicroseconds(1);

  return data;
}

// ---------------------------------------------------------------------------------------------------------

uint16_t write_data(uint32_t address, uint16_t data, int ic_no) {
  if (ic_no == 0) {
    digitalWrite(IC1_WE_n, HIGH);
    delayMicroseconds(1);
  } else {
    digitalWrite(IC2_WE_n, HIGH);
    delayMicroseconds(1);
  }

  digitalWrite(FLASH_A00, (address >> 0) & 1);
  digitalWrite(FLASH_A01, (address >> 1) & 1);
  digitalWrite(FLASH_A02, (address >> 2) & 1);
  digitalWrite(FLASH_A03, (address >> 3) & 1);
  digitalWrite(FLASH_A04, (address >> 4) & 1);
  digitalWrite(FLASH_A05, (address >> 5) & 1);
  digitalWrite(FLASH_A06, (address >> 6) & 1);
  digitalWrite(FLASH_A07, (address >> 7) & 1);
  digitalWrite(FLASH_A08, (address >> 8) & 1);
  digitalWrite(FLASH_A09, (address >> 9) & 1);
  digitalWrite(FLASH_A10, (address >> 10) & 1);
  digitalWrite(FLASH_A11, (address >> 11) & 1);
  digitalWrite(FLASH_A12, (address >> 12) & 1);
  digitalWrite(FLASH_A13, (address >> 13) & 1);
  digitalWrite(FLASH_A14, (address >> 14) & 1);
  digitalWrite(FLASH_A15, (address >> 15) & 1);
  digitalWrite(FLASH_A16, (address >> 16) & 1);
  digitalWrite(FLASH_A17, (address >> 17) & 1);
  digitalWrite(FLASH_A18, (address >> 18) & 1);
  digitalWrite(FLASH_A19, (address >> 19) & 1);
  digitalWrite(FLASH_A20, (address >> 20) & 1);
  digitalWrite(FLASH_A21, (address >> 21) & 1);
  digitalWrite(FLASH_A22, (address >> 22) & 1);

  digitalWrite(FLASH_DQ00, (data >> 0) & 1);
  digitalWrite(FLASH_DQ01, (data >> 1) & 1);
  digitalWrite(FLASH_DQ02, (data >> 2) & 1);
  digitalWrite(FLASH_DQ03, (data >> 3) & 1);
  digitalWrite(FLASH_DQ04, (data >> 4) & 1);
  digitalWrite(FLASH_DQ05, (data >> 5) & 1);
  digitalWrite(FLASH_DQ06, (data >> 6) & 1);
  digitalWrite(FLASH_DQ07, (data >> 7) & 1);
  digitalWrite(FLASH_DQ08, (data >> 8) & 1);
  digitalWrite(FLASH_DQ09, (data >> 9) & 1);
  digitalWrite(FLASH_DQ10, (data >> 10) & 1);
  digitalWrite(FLASH_DQ11, (data >> 11) & 1);
  digitalWrite(FLASH_DQ12, (data >> 12) & 1);
  digitalWrite(FLASH_DQ13, (data >> 13) & 1);
  digitalWrite(FLASH_DQ14, (data >> 14) & 1);
  digitalWrite(FLASH_DQ15, (data >> 15) & 1);

  if (ic_no == 0) {
    digitalWrite(IC1_WE_n, LOW);
    delayMicroseconds(1);
    digitalWrite(IC1_WE_n, HIGH);
    delayMicroseconds(1);
  } else {
    digitalWrite(IC2_WE_n, LOW);
    delayMicroseconds(1);
    digitalWrite(IC2_WE_n, HIGH);
    delayMicroseconds(1);
  }

  return data;
}

// ---------------------------------------------------------------------------------------------------------

void print_hex_dump(uint32_t startAddress, uint32_t endAddress) {
  // Assuming each line of the dump is 16 bytes wide
  int bytesPerLine = 16;
  uint32_t crc32 = INITIAL_VALUE;

  // Print header
  SerialUSB.println("HEX DUMP STARTING FROM ADDRESS 0x" + pad_str(startAddress, 6) + " to 0x" + pad_str(endAddress, 6));
  SerialUSB.print("\n");
  SerialUSB.println("MEMORY  0000 0001 0002 0003 0004 0005 0006 0007 0008 0009 000A 000B 000C 000D 000E 000F  BYTES AS ASCII");
  SerialUSB.println("------  -------------------------------------------------------------------------------  -------------------------------");

  // Iterate over each line
  for (uint32_t address = startAddress; address <= endAddress; address += bytesPerLine) {
    // Print address at the start of the line
    String addressStr = pad_str(address, 6);

    // Print padded address at the start of the line
    SerialUSB.print(addressStr);
    SerialUSB.print("  ");

    String asciiRepresentation = "";

    uint16_t data = 0;
    char lsb, msb;

    // Iterate over each byte in the line
    for (int i = 0; i < bytesPerLine; ++i) {
      // Read data from flash memory
      data = read_data(address + i);

      lsb = (char)(data & 0xFF);
      msb = (char)((data >> 8) & 0xFF);

      crc32 = updateCRC32((uint8_t)lsb, crc32);
      crc32 = updateCRC32((uint8_t)msb, crc32);

      // Print hex value
      SerialUSB.print(pad_str(data, 4));
      SerialUSB.print(" ");

      // Convert byte to ASCII character or '.' for non-printable characters
      uint8_t data_low = (data >> 8) & 0xFF;
      uint8_t data_high = data & 0xFF;
      asciiRepresentation += (char)((data_high >= 32 && data_high <= 126) ? data_high : '.');
      asciiRepresentation += (char)((data_low >= 32 && data_low <= 126) ? data_low : '.');
    }

    // Ensure ASCII representation is exactly 16 characters wide
    while (asciiRepresentation.length() < bytesPerLine) {
      asciiRepresentation += " ";  // Pad with spaces if necessary
    }

    // Print ASCII representation
    SerialUSB.print(" ");
    SerialUSB.println(asciiRepresentation);
  }

  // Calculate and print checksum (dummy value shown here)
  SerialUSB.print("\n");
  String crcStr = pad_str(~crc32, 8);

  SerialUSB.println("CRC32 CHECKSUM FOR DUMP BLOCK 0x" + String(startAddress, HEX) + " to 0x" + String(endAddress, HEX) + " IS: 0x" + String(crcStr));
}

// ---------------------------------------------------------------------------------------------------------

void print_dump(uint32_t startAddress, uint32_t endAddress) {
  for (uint32_t address = startAddress; address < endAddress; address++) {
    uint16_t data = read_data(address);
    SerialUSB.write((const uint8_t *)&data, sizeof(data));
  }
}

// ---------------------------------------------------------------------------------------------------------

void read_data_polling_register_program(uint32_t address, bool written_dq07) {
  configure_datapins_read();
  delayMicroseconds(1);
  digitalWrite(FLASH_OE_n, LOW);
  delayMicroseconds(1);

  do {
      bool dq07 = read_data_same_address_dq07();
//      Serial.print("  poll dq07: ");
//      Serial.println(dq07);
      
      if (dq07 == written_dq07) { // if dq07 is the written_dq07, writing is done, we can proceed
//        Serial.println("    match, next byte");
        break;
      }
      // otherwise it's the complement and we need to wait some more
//      Serial.println("    complement, repeat");
  } while (true);

  digitalWrite(FLASH_OE_n, HIGH);
  delayMicroseconds(1);
  configure_datapins_write();
}

// ---------------------------------------------------------------------------------------------------------

void read_data_polling_register_erase(uint32_t address) {
  configure_datapins_read();
  delayMicroseconds(100);
  digitalWrite(FLASH_OE_n, LOW);
  delayMicroseconds(100);

  do {
      uint16_t currentValue = read_data(address);
      uint16_t dq07 = (currentValue >> 7) & 1;
      uint16_t dq06 = (currentValue >> 6) & 1;
      uint16_t dq05 = (currentValue >> 5) & 1;
      uint16_t dq03 = (currentValue >> 3) & 1;
      uint16_t dq02 = (currentValue >> 2) & 1;
      uint16_t dq01 = (currentValue >> 1) & 1;
/*
      SerialUSB.print("dq07: ");
      SerialUSB.print(dq07);
      SerialUSB.print(" / dq06: ");
      SerialUSB.print(dq06);
      SerialUSB.print(" / dq05: ");
      SerialUSB.print(dq05);
      SerialUSB.print(" / dq03: ");
      SerialUSB.print(dq03);
      SerialUSB.print(" / dq02: ");
      SerialUSB.print(dq02);
      SerialUSB.print(" / dq01: ");
      SerialUSB.println(dq01);
      
      delay(10);
*/
      if (dq07 == 0) { // erase still in progress
        continue;
      }

      if (dq07 == 1) { // erase completed
        break;
      }

  } while (true);

  digitalWrite(FLASH_OE_n, HIGH);
  delayMicroseconds(100);
  digitalWrite(FLASH_IC1_CE_n, HIGH);
  delayMicroseconds(100);
  digitalWrite(FLASH_IC2_CE_n, HIGH);
  delayMicroseconds(100);
}

// ---------------------------------------------------------------------------------------------------------

void flash_reset() {
  digitalWrite(FLASH_RST_n, HIGH);
  delayMicroseconds(100);
  digitalWrite(FLASH_RST_n, LOW);
  delayMicroseconds(100);
  digitalWrite(FLASH_RST_n, HIGH);
  delayMicroseconds(100);
}

// ---------------------------------------------------------------------------------------------------------

void chiperase(int ic_no) {
      configure_datapins_write();
      digitalWrite(FLASH_RST_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_RST_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_RST_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_WP_n, HIGH);
      delayMicroseconds(100);

      digitalWrite(FLASH_OE_n, HIGH);
      delayMicroseconds(100);
      if (ic_no == 0) {
        digitalWrite(FLASH_IC1_CE_n, LOW);
        delayMicroseconds(100);
        digitalWrite(FLASH_IC2_CE_n, HIGH);
        delayMicroseconds(100);
      } else {
        digitalWrite(FLASH_IC1_CE_n, HIGH);
        delayMicroseconds(100);
        digitalWrite(FLASH_IC2_CE_n, LOW);
        delayMicroseconds(100);
      }

      // chiperase
      write_data(0x555, 0xaa, ic_no);
      write_data(0x2aa, 0x55, ic_no);
      write_data(0x555, 0x80, ic_no);
      write_data(0x555, 0xaa, ic_no);
      write_data(0x2aa, 0x55, ic_no);
      write_data(0x555, 0x10, ic_no);

      read_data_polling_register_erase(0);
}

// ---------------------------------------------------------------------------------------------------------


bool blank_check(uint32_t block, int ic_no) { // returns true if success, false if failure
    configure_datapins_write();
    digitalWrite(FLASH_RST_n, HIGH);
    delayMicroseconds(100);
    digitalWrite(FLASH_WP_n, HIGH);
    delayMicroseconds(100);
    digitalWrite(FLASH_OE_n, HIGH);
    delayMicroseconds(100);

    if (ic_no == 0) {
      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);
    } else {
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, LOW);
      delayMicroseconds(100);
    }

    write_data(0x555, 0xaa, ic_no);
    write_data(0x2aa, 0x55, ic_no);
    write_data(block<<16, 0xeb, ic_no);
    write_data(block<<16, 0x76, ic_no);
    write_data(block<<16, 0x00, ic_no);
    write_data(block<<16, 0x00, ic_no);
    write_data(block<<16, 0x29, ic_no);

    configure_datapins_read();
    digitalWrite(FLASH_OE_n, LOW);
    delayMicroseconds(100);

    bool blankcheck_status = true;

    uint16_t dq07, dq06, dq05, dq01;
    uint16_t previousValue = read_data(block<<16);
    uint16_t previousDq06 = (previousValue >> 6) & 1;
    bool processSuccess = true;
    do {
        uint16_t currentValue = read_data(block<<16);
        dq07 = (currentValue >> 7) & 1;
        dq06 = (currentValue >> 6) & 1;
        dq05 = (currentValue >> 5) & 1;
        dq01 = (currentValue >> 1) & 1;
/*
        SerialUSB.print("DQ07: ");
        SerialUSB.print(dq07);
        SerialUSB.print(" / DQ06: ");
        SerialUSB.print(dq06);
        SerialUSB.print(" / DQ05: ");
        SerialUSB.println(dq05);
*/
        if (previousDq06 == dq06) {
          break;
        }

        if (dq05 != 1 && dq01 != 1) {
          continue;
        }

        previousValue = read_data(block<<16);
        previousDq06 = (previousValue >> 6) & 1;
        currentValue = read_data(block<<16);
        dq07 = (currentValue >> 7) & 1;
        dq06 = (currentValue >> 6) & 1;
        dq05 = (currentValue >> 5) & 1;
        dq01 = (currentValue >> 1) & 1;

        if (previousDq06 != dq06)
          processSuccess = false;

        break;
    } while (true);

//    SerialUSB.print("processSuccess: ");
//    SerialUSB.println(processSuccess);

    if (dq05 == 1)
      blankcheck_status = false;

    digitalWrite(FLASH_IC1_CE_n, HIGH);
    delayMicroseconds(100);
    digitalWrite(FLASH_IC2_CE_n, HIGH);
    delayMicroseconds(100);
    digitalWrite(FLASH_OE_n, HIGH);
    delayMicroseconds(100);

    return blankcheck_status;
}

void buffered_program_received_data(int ic_no) {
      static uint8_t data_buffer[BLOCK_SIZE];
      static uint32_t words_per_ic = 8388608;
      uint16_t word = 0;
      uint32_t address_counter = 0;

      // unlock bypass
      write_data(0x555, 0xaa, ic_no);
      write_data(0x2aa, 0x55, ic_no);
      write_data(0x555, 0x20, ic_no);

      while (!SerialUSB.available()) {}

      address_counter = 0;
      //SerialUSB.println('R');
      while (address_counter < words_per_ic) {
        SerialUSB.readBytes(data_buffer, BLOCK_SIZE);

        // unlock bypass write to buffer program
        write_data(address_counter, 0x25, ic_no);
        write_data(address_counter, (BLOCK_SIZE/2)-1, ic_no);

        for (int i=0; i < BLOCK_SIZE; i+=2) {
          word = 0;
          word = data_buffer[i];
          word |= data_buffer[i+1] << 8;

          write_data(address_counter, word, ic_no);
          address_counter++;
        }
        // write to buffer confirm
        write_data(address_counter-1, 0x29, ic_no);

//        Serial.print("dq07: ");
//        Serial.println((word >> 7 )&1);

        read_data_polling_register_program(address_counter-1, (word >> 7 )&1);
      }
      
      // reset unlock bypass
      write_data(0, 0x90, ic_no);
      write_data(0, 0x00, ic_no);
}

// ---------------------------------------------------------------------------------------------------------

uint32_t calc_crc32(uint32_t startAddress, uint32_t endAddress, uint32_t crc32) {
  int last_progress_value = -1;
  uint16_t data = 0;
  char lsb, msb;
  for (uint32_t address = startAddress; address < endAddress; address++) {
    last_progress_value = display_progress_bar(address - startAddress, endAddress - startAddress, last_progress_value);
    data = read_data(address);
    lsb = (char)(data & 0xFF);
    msb = (char)((data >> 8) & 0xFF);

    crc32 = updateCRC32((uint8_t)lsb, crc32);
    crc32 = updateCRC32((uint8_t)msb, crc32);
  }
  return crc32;
}

// =========================================================================================================
// Main loop
// =========================================================================================================

void loop() {
  static String inputString = "";         // a String to hold incoming data
  static boolean stringComplete = false;  // whether the string is complete

  static unsigned long lastDebounceTime01 = 0;
  static unsigned long lastDebounceTime02 = 0;
  static unsigned long lastDebounceTime03 = 0;
  bool lastButton01State = HIGH;
  bool lastButton02State = HIGH;
  bool lastButton03State = HIGH;
  unsigned long debounceDelay = 300;

  bool currentButton01State = digitalRead(BUTTON01);
  bool currentButton02State = digitalRead(BUTTON02);
  bool currentButton03State = digitalRead(BUTTON03);

  if ((lastButton01State == HIGH) && (currentButton01State == LOW) && ((millis() - lastDebounceTime01) > debounceDelay)) {
    SerialUSB.println("Button 01 pressed");
    lastDebounceTime01 = millis();
  }

  if ((lastButton02State == HIGH) && (currentButton02State == LOW) && ((millis() - lastDebounceTime02) > debounceDelay)) {
    SerialUSB.println("Button 02 pressed");
    lastDebounceTime02 = millis();
  }

  if ((lastButton03State == HIGH) && (currentButton03State == LOW) && ((millis() - lastDebounceTime03) > debounceDelay)) {
    SerialUSB.println("Button 03 pressed");
    lastDebounceTime03 = millis();
  }

  if (SerialUSB.available() > 0) {
    char inChar = SerialUSB.read();  // Read the incoming character
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // otherwise add it to the inputString:
      inputString += inChar;
    }
  }

  // check if the string is complete
  if (stringComplete) {
    inputString.trim();
    inputString.toLowerCase();
    // Check if the received character is 'V' or 'v'
    if (inputString.equals("v")) {
      // Send the version number back to the computer
      SerialUSB.println("Version 0.1");
    } else if (inputString.startsWith("hdp1 ")) {
      configure_datapins_read();
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);

      // remove the command part ("hdp ")
      String args = inputString.substring(5);

      // find the space between the addresses
      int spaceIndex = args.indexOf(' ');
      if (spaceIndex != -1) {

        // parse the start address
        String startAddrStr = args.substring(0, spaceIndex);
        uint32_t startAddress = strtoul(startAddrStr.c_str(), NULL, 16);

        // parse the end address
        String endAddrStr = args.substring(spaceIndex + 1);
        uint32_t endAddress = strtoul(endAddrStr.c_str(), NULL, 16);

        digitalWrite(FLASH_IC1_CE_n, LOW);
        delayMicroseconds(100);

        // call the function to print the hex dump
        print_hex_dump(startAddress, endAddress);

        // reset chip enable pins
        digitalWrite(FLASH_IC1_CE_n, HIGH);
        delayMicroseconds(100);
        digitalWrite(FLASH_IC2_CE_n, HIGH);
        delayMicroseconds(100);
      }
    } else if (inputString.startsWith("hdp2 ")) {
      configure_datapins_read();
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      // remove the command part ("hdp ")
      String args = inputString.substring(5);

      // find the space between the addresses
      int spaceIndex = args.indexOf(' ');
      if (spaceIndex != -1) {

        // parse the start address
        String startAddrStr = args.substring(0, spaceIndex);
        uint32_t startAddress = strtoul(startAddrStr.c_str(), NULL, 16);

        // parse the end address
        String endAddrStr = args.substring(spaceIndex + 1);
        uint32_t endAddress = strtoul(endAddrStr.c_str(), NULL, 16);

        digitalWrite(FLASH_IC2_CE_n, LOW);
        delayMicroseconds(100);

        // call the function to print the hex dump
        print_hex_dump(startAddress, endAddress);

        // reset chip enable pins
        digitalWrite(FLASH_IC1_CE_n, HIGH);
        delayMicroseconds(100);
        digitalWrite(FLASH_IC2_CE_n, HIGH);
        delayMicroseconds(100);
      }
    } else if (inputString.startsWith("dp all")) {
      configure_datapins_read();
      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      print_dump(0, 8388608);
      delayMicroseconds(500);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delay(5);

      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, LOW);
      delayMicroseconds(100);
      print_dump(0, 8388608);
      delayMicroseconds(500);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delay(3);
    } else if (inputString.startsWith("dp1 all")) {
      configure_datapins_read();
      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      print_dump(0, 8388608);
      delayMicroseconds(500);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delay(3);
    } else if (inputString.startsWith("dp2 all")) {
      configure_datapins_read();
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      print_dump(0, 8388608);
      delayMicroseconds(500);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delay(3);
    } else if (inputString.startsWith("crc all")) {
      configure_datapins_read();
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      SerialUSB.println("Calculating CRC32, this might take a while. Be patient...");
      SerialUSB.println("Calculating checksum for IC 1 of 2:");
      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      uint32_t crc32 = INITIAL_VALUE;
      crc32 = calc_crc32(0, 8388608, crc32);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);

      SerialUSB.print("CRC32 IC1: 0x");
      SerialUSB.println(~crc32, HEX);

      SerialUSB.println("Calculating checksum for IC 2 of 2:");
      digitalWrite(FLASH_IC2_CE_n, LOW);
      delayMicroseconds(100);
      crc32 = calc_crc32(0, 8388608, crc32);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);

      crc32 = ~crc32;
      SerialUSB.print("CRC32 (both ICs): 0x");
      SerialUSB.println(crc32, HEX);
      SerialUSB.print("done");
    } else if (inputString.startsWith("crc1 all")) {
      configure_datapins_read();
      digitalWrite(FLASH_OE_n, LOW);
      delayMicroseconds(100);
      SerialUSB.println("Calculating CRC32 for IC1, this might take a while. Be patient...");
      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      uint32_t crc32 = INITIAL_VALUE;
      crc32 = calc_crc32(0, 8388608, crc32);
      digitalWrite(FLASH_IC1_CE_n, HIGH);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);

      crc32 = ~crc32;
      SerialUSB.print("CRC32: 0x");
      SerialUSB.println(crc32, HEX);
      SerialUSB.print("done");
    } else if (inputString.startsWith("er all")) {
      chiperase(0);
      chiperase(1);

      digitalWrite(FLASH_RST_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_RST_n, HIGH);
      delayMicroseconds(100);

      SerialUSB.println("done");
    } else if (inputString.startsWith("wr1 all")) {
      SerialUSB.setTimeout(25000); 

      configure_datapins_write();
      digitalWrite(FLASH_OE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_RST_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_WP_n, HIGH);
      delayMicroseconds(100);

      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);

      buffered_program_received_data(0);

      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);
    } else if (inputString.startsWith("wr all")) {
      SerialUSB.setTimeout(25000); 

      configure_datapins_write();
      digitalWrite(FLASH_OE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_RST_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_WP_n, HIGH);
      delayMicroseconds(100);

      digitalWrite(FLASH_IC1_CE_n, LOW);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);

      buffered_program_received_data(0);

      delay(100);

      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, LOW);
      delayMicroseconds(100);

      buffered_program_received_data(1);

      digitalWrite(FLASH_IC1_CE_n, HIGH);
      delayMicroseconds(100);
      digitalWrite(FLASH_IC2_CE_n, HIGH);
      delayMicroseconds(100);
    } else if (inputString.startsWith("bc all")) {
        bool result = true;

        for (int i=0; i < 127; i++) {
//          SerialUSB.print("block ");
//          SerialUSB.println(i);
          result = blank_check(i, 0);
//          SerialUSB.print("  blankcheck: ");
//          SerialUSB.println(result);

          if (result == false) {
            SerialUSB.print("block ");
            SerialUSB.print(i);
            SerialUSB.print(" (0x");
            SerialUSB.print(pad_str(i<<16,7));  //,HEX);
            SerialUSB.print(" - 0x");
            SerialUSB.print(pad_str((i<<16) + 0xFFFF,7));  //,HEX);
            SerialUSB.print(") ");
            SerialUSB.println("is not blank!");
          }
          
        }

        for (int i=0; i < 127; i++) {
//          SerialUSB.print("block ");
//          SerialUSB.println(i);
          result = blank_check(i, 1);
//          SerialUSB.print("  blankcheck: ");
//          SerialUSB.println(result);

          if (result == false) {
            SerialUSB.print("block ");
            SerialUSB.print(i);
            SerialUSB.print(" (0x");
            SerialUSB.print(pad_str(i<<16,7));  //,HEX);
            SerialUSB.print(" - 0x");
            SerialUSB.print(pad_str((i<<16) + 0xFFFF,7));  //,HEX);
            SerialUSB.print(") ");
            SerialUSB.println("is not blank!");
          }          
        }

        SerialUSB.println("done");
        delayMicroseconds(100);
        digitalWrite(FLASH_IC1_CE_n, HIGH);
        delayMicroseconds(100);
        digitalWrite(FLASH_IC2_CE_n, HIGH);
        delayMicroseconds(100);

        flash_reset();
      }

    // clear the string for new input:
    inputString = "";
    stringComplete = false;
  }
  lastButton01State = currentButton01State;
  lastButton02State = currentButton02State;
  lastButton03State = currentButton03State;
}
