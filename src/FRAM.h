#include <inttypes.h>
#include <stdlib.h>

typedef enum command {
    WRSR    = 0x1,
    WRITE   = 0x2,
    READ    = 0x3,
    WRDI    = 0x4,
    RDSR    = 0x5,
    WREN    = 0x6,
    FSTRD   = 0xb,
    RDID    = 0x9f,
    SLEEP   = 0xb9,
} cmd_t;

class FRAM {
    public:
        FRAM(int chipSelectPin, int writeProtectPin);
        ~FRAM();
        void begin(void);
        uint8_t getManufacturerID(void);
        uint8_t getChipDensity(void);
        uint32_t getSizeInBytes(void);

        uint8_t *read(uint32_t address, uint8_t *b);
        unsigned char *read(uint32_t address, uint8_t *buf, size_t len);

        bool write(uint32_t address, uint8_t b);
        bool write(uint32_t address, const uint8_t *buf, size_t len);

        uint8_t readStatusRegister(void);
        bool isStatusRegisterWriteProtected(void);

        void enableWrite(void);
        void disableWrite(void);
        bool isWritable(void);

        void sendCommand(cmd_t command);
        void beginTransaction(void);
        void endTransaction(void);
    private:
        uint32_t _size;
        int _csPin;
        int _wpPin;
};
