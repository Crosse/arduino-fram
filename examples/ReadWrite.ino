#include <inttypes.h>
#include <stdbool.h>

#include <SPI.h>

#include <FRAM.h>

const int pinWP = 9;
const int pinChipSelect = 10;

FRAM fram(pinChipSelect, pinWP);

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        delay(100);
    Serial.println(F("\n\nconnected"));
    
    SPI.begin();
    fram.begin();

    readByte(0);
    writeByte(0);
    readByte(0);
    Serial.println();

    Serial.println("enabling writes");
    fram.enableWrite();
    readStatus();
    Serial.println();

    Serial.println("disabling writes");
    fram.disableWrite();
    readStatus();
}

void loop() { }

void readStatus()
{
    uint8_t sr = fram.readStatusRegister();
    Serial.print("Status register: 0b");
    Serial.println(sr, BIN);
    Serial.println("status register write protected? " + 
            String((fram.isStatusRegisterWriteProtected() ? "yes" : "no")));
    Serial.println("write enabled? " + 
            String((fram.isWritable() ? "yes" : "no")));
}

void readByte(uint32_t address)
{
    Serial.print("Reading a byte at offset " + String(address) + ": ");
    uint8_t b = 0;
    if (fram.read(address, &b) == NULL)
        Serial.println("failed!");
    else
        Serial.println(String(b));

}

void writeByte(uint32_t address)
{
    Serial.print("Inverting byte at offset " + String(address) + ": ");
    uint8_t b = 0;
    fram.read(address, &b);
    if (fram.write(address, !b) == false)
        Serial.println("failed!");
    else
        Serial.println("success");
}
