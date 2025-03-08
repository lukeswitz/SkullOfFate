#ifndef NFCWRITER_H
#define NFCWRITER_H

#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_ST25DV64KC_Arduino_Library.h"

class NFCWriter {
public:
    NFCWriter(SFE_ST25DV64KC &tag);
    bool initializeTag();
    void wipeEEPROM();
    void writeCCFile();
    void writeRandomURI(bool fullRange = false);
    void verifyWrite(uint16_t totalSize);
    uint16_t getTotalSize();
private:
    SFE_ST25DV64KC &tag;
    uint16_t memLoc;
    void writeEEPROMChunked(uint16_t address, uint8_t *data, uint16_t length);
};

void prettyPrintChars(uint8_t *theData, int theLength);

#endif // NFCWRITER_H