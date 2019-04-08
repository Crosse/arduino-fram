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

    Serial.println("starting time trials");

    uint8_t buf[1024] = { 0 };

    timeTrial(buf, 1);
    timeTrial(buf, 8);
    timeTrial(buf, 16);
    timeTrial(buf, 64);
    timeTrial(buf, 128);
    timeTrial(buf, 256);
    timeTrial(buf, 512);
    timeTrial(buf, 1024);
}

void loop() { delay(1000); }

void timeTrial(uint8_t *buf, uint32_t len)
{
    uint32_t bytes = 0;
    uint32_t xfers = fram.getSizeInBytes() / len;
    
    Serial.print("Starting time trial; will perform ");
    Serial.print(String(len) + "-byte transfers (");
    Serial.println(String(xfers) + " total transfers)");

    uint32_t start = millis();
    for (uint32_t i = 0; i < xfers; i++) {
        fram.write(i, buf, len);
        bytes += len;
    }
    uint32_t end = millis();
    Serial.println("==> wrote " + String(bytes) + " bytes.");

    float bps = fram.getSizeInBytes() / ((end - start) / 1000.0);
    Serial.print("==> write took " + String(end - start) + "ms at ");
    Serial.println(String(bps, 2) + " bytes per second)\n");
}

