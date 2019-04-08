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

    Serial.println("Manufacturer ID: " + String(fram.getManufacturerID()));
    Serial.println("Number of modules: " + String(fram.getChipDensity()));
    Serial.println("Size: " + String(fram.getSizeInBytes()) + " bytes");
    Serial.println();

    uint8_t sr = fram.readStatusRegister();
    Serial.print("Status register: 0b");
    Serial.println(sr, BIN);
    Serial.println("status register write protected? " + 
            String((fram.isStatusRegisterWriteProtected() ? "yes" : "no")));
    Serial.println("write enabled? " + 
            String((fram.isWritable() ? "yes" : "no")));
    Serial.println();
}

void loop() { }
