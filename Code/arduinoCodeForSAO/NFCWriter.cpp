#include "NFCWriter.h"

NFCWriter::NFCWriter(SFE_ST25DV64KC &tagRef) : tag(tagRef), memLoc(0x00) {
}

bool configureGPO1 = true;

bool NFCWriter::initializeTag() {
    // Initialize the RFID tag over I2C
    if (!tag.begin(Wire)) {
        Serial.println(F("ST25DV64KC not detected. Halting..."));
        return false;
    }
    Serial.println(F("ST25DV64KC detected and connected."));

    if (configureGPO1){
        // The GPO registers can only be changed during an open security session
        Serial.println(F("Opening I2C session with password."));
        uint8_t password[8] = {0x0};
        tag.openI2CSession(password);
        
        Serial.print(F("I2C session is "));
        Serial.println(tag.isI2CSessionOpen() ? "opened." : "closed.");
        
        Serial.println(F("Configuring GPO1 on activity."));
        tag.setGPO1Bit(BIT_GPO1_FIELD_CHANGE_EN, true);
        //Serial.println(F("Disabling other bits."));
        tag.setGPO1Bit(BIT_GPO1_RF_USER_EN, true);
        tag.setGPO1Bit(BIT_GPO1_RF_ACTIVITY_EN, true);
        tag.setGPO1Bit(BIT_GPO1_RF_INTERRUPT_EN, true);
        tag.setGPO1Bit(BIT_GPO1_RF_PUT_MSG_EN, true);
        tag.setGPO1Bit(BIT_GPO1_RF_GET_MSG_EN, true);
        tag.setGPO1Bit(BIT_GPO1_RF_WRITE_EN, true);
        
        Serial.println(F("Enabling GPO_EN bit."));
        tag.setGPO1Bit(BIT_GPO1_GPO_EN, true);
        // Note: the GPO Control Dynamic Bit can be set or cleared at any time
        //tag.setGPO_CTRL_DynBit(false); // This will disable GPO even when I2C security is closed
    }
    return true;
}

void NFCWriter::wipeEEPROM() {
    Serial.println(F("Wiping EEPROM by writing zeros..."));

    // Define the total EEPROM size
    const uint16_t EEPROM_SIZE = 2048;  // 2 KB

    // Create a buffer filled with zeros
    uint8_t zeroBuffer[16];  // Buffer size of 16 bytes
    memset(zeroBuffer, 0x00, sizeof(zeroBuffer));

    // Write zeros to EEPROM in chunks
    uint16_t address = 0x0000;
    while (address < EEPROM_SIZE) {
        uint8_t bytesToWrite = min(sizeof(zeroBuffer), EEPROM_SIZE - address);
        tag.writeEEPROM(address, zeroBuffer, bytesToWrite);
        address += bytesToWrite;
    }

    Serial.println(F("EEPROM wiped."));
}

void NFCWriter::writeCCFile() {
    // Write the Correct CC file (Capability Container)
   
    /*
    uint8_t ccFile[] = {
        0xE1,  // Magic number for Type 5 tag
        0x10,  // Version and access
        0x00,  // Reserved
        0x02,  // Memory size in multiples of 8 bytes (2 * 8 = 16 bytes)
        0x00,  // Reserved
        0x00,  // Reserved
        0x03, 0xFE  // NDEF TLV block size (0xFE indicates terminator TLV follows)
    };
*/

    uint8_t ccFile[] = {
        0xE2,  // Magic number for Type 2 tag
        0x40,  // Version and access
        0x00,  // Reserved
        0x01,  // Memory size in multiples of 8 bytes (1 * 8 = 8 bytes)
        0x00,  // Reserved
        0x00,  // Reserved
        0x03, 0xFF  // NDEF TLV block size
    };

    Serial.println(F("Writing CC_File to memory address 0x00"));
    tag.writeEEPROM(0x00, ccFile, sizeof(ccFile));

    // Manually set the memory location to 0x08, right after the CC file
    //memLoc = 0x08;

    memLoc = 0x10;

}

void NFCWriter::writeRandomURI() {
    // Generate a random number between 1 and 78 (inclusive)
    uint8_t randomNumber = random(1, 79);  // low inclusive, high exclusive

    //randomNumber = 3;
    
    // Construct the URI string
    char uriString[100];
    snprintf(uriString, sizeof(uriString), "github.com/lukeswitz/SkullOfFate/blob/main/Readings/%u.md", randomNumber);
    
    // Debug: Print the constructed URI
    Serial.print(F("Generated URI: "));
    Serial.println(uriString);

    // Create the payload
    // URI Identifier Code for "https://" is 0x04
    const char* uriPrefix = "https://";
    uint8_t uriIdentifierCode = 0x04;

    // Calculate the payload length
    uint16_t payloadLength = 1 + strlen(uriString);  // 1 byte for identifier code

    // Create the NDEF record
    uint8_t ndefRecord[1 + 1 + 1 + 1 + 1 + strlen(uriString)];  // Header + Type Length + Payload Length + Type + Identifier + URI
    uint16_t ndefRecordIndex = 0;

    // Header: MB=1, ME=1, SR=1, TNF=0x01 (Well-known type)
    ndefRecord[ndefRecordIndex++] = 0xD1;

    // Type Length: 1 (for 'U')
    ndefRecord[ndefRecordIndex++] = 0x01;

    // Payload Length: 1 (identifier) + length of URI string without prefix
    ndefRecord[ndefRecordIndex++] = payloadLength;

    // Type: 'U' (0x55)
    ndefRecord[ndefRecordIndex++] = 0x55;

    // Payload: Identifier Code + URI string without prefix
    ndefRecord[ndefRecordIndex++] = uriIdentifierCode;
    memcpy(&ndefRecord[ndefRecordIndex], uriString, strlen(uriString));
    ndefRecordIndex += strlen(uriString);

    // Calculate the total NDEF message length
    uint16_t totalNDEFLength = ndefRecordIndex;

    // TLV Block
    uint8_t tlvBlock[4];
    int tlvBlockLength = 0;
    if (totalNDEFLength < 255) {
        tlvBlock[0] = 0x03;        // NDEF message type
        tlvBlock[1] = (uint8_t)totalNDEFLength;   // Length of the NDEF message
        tlvBlockLength = 2;
    } else {
        tlvBlock[0] = 0x03;        // NDEF message type
        tlvBlock[1] = 0xFF;        // Length field indicator for extended length
        tlvBlock[2] = (uint8_t)((totalNDEFLength >> 8) & 0xFF);  // High byte of length
        tlvBlock[3] = (uint8_t)(totalNDEFLength & 0xFF);         // Low byte of length
        tlvBlockLength = 4;
    }
    writeEEPROMChunked(memLoc, tlvBlock, tlvBlockLength);
    memLoc += tlvBlockLength;

    // Write the NDEF record
    writeEEPROMChunked(memLoc, ndefRecord, totalNDEFLength);
    memLoc += totalNDEFLength;

    // Add terminator TLV (0xFE)
    uint8_t terminatorTLV = 0xFE;
    Serial.println(F("Writing Terminator TLV to memory."));
    writeEEPROMChunked(memLoc, &terminatorTLV, 1);
    memLoc += 1;

    Serial.println(F("URI written successfully."));
    
    // Verify the written data
    verifyWrite(memLoc);
}

void NFCWriter::verifyWrite(uint16_t totalSize) {
    Serial.println(F("Reading back NDEF messages from memory."));
    uint8_t readBack[totalSize];
    tag.readEEPROM(0x00, readBack, totalSize);

    // Print the read-back data
    prettyPrintChars(readBack, totalSize);
}

uint16_t NFCWriter::getTotalSize() {
    return memLoc;
}

// Helper function to write data to EEPROM in chunks
void NFCWriter::writeEEPROMChunked(uint16_t address, uint8_t *data, uint16_t length) {
    const uint8_t chunkSize = 16;  // Use 16 bytes per write to be safe
    uint16_t bytesWritten = 0;
    while (bytesWritten < length) {
        uint8_t bytesToWrite = min(chunkSize, (uint8_t)(length - bytesWritten));
        tag.writeEEPROM(address + bytesWritten, data + bytesWritten, bytesToWrite);
        bytesWritten += bytesToWrite;
    }
}

// Helper function to print data in HEX and ASCII format
void prettyPrintChars(uint8_t *theData, int theLength) {
    Serial.println();  // Print a new line for formatting

    // Loop over the data in blocks of 16 bytes
    for (int i = 0; i < theLength; i += 16) {
        // Print the offset of the current block (4 digits with leading zeros)
        if (i < 10000) Serial.print(F("0"));
        if (i < 1000) Serial.print(F("0"));
        if (i < 100) Serial.print(F("0"));
        if (i < 10) Serial.print(F("0"));
        Serial.print(i);

        // Print the memory address in HEX format (with leading zeros)
        Serial.print(F(" 0x"));
        if (i < 0x1000) Serial.print(F("0"));
        if (i < 0x100) Serial.print(F("0"));
        if (i < 0x10) Serial.print(F("0"));
        Serial.print(i, HEX);

        Serial.print(F(" "));

        // Print the data in HEX format (16 bytes per row)
        int j;
        for (j = 0; ((i + j) < theLength) && (j < 16); j++) {
            if (theData[i + j] < 0x10) Serial.print(F("0"));
            Serial.print(theData[i + j], HEX);
            Serial.print(F(" "));
        }

        // Print padding if this is the last block and it contains fewer than 16 bytes
        if (((i + j) == theLength) && (j < 16)) {
            for (int k = 0; (k < (16 - (theLength % 16))); k++) {
                Serial.print(F("   "));
            }
        }

        // Print the data as ASCII characters (print '.' for non-printable characters)
        for (j = 0; ((i + j) < theLength) && (j < 16); j++) {
            if ((theData[i + j] >= 0x20) && (theData[i + j] <= 0x7E))
                Serial.write(theData[i + j]);  // Print the ASCII character if it is printable
            else
                Serial.print(F("."));  // Print a dot for non-printable characters
        }

        Serial.println();  // Print a new line at the end of each row
    }

    Serial.println();  // Print a final new line after all data is printed
}
