// ConfigManager.cpp
#include "ConfigManager.h"

// Constructor implementation
ConfigManager::ConfigManager(Adafruit_SPIFlash& flashRef) : flash(flashRef), fatfs() {}

// Initialize ConfigManager
bool ConfigManager::initialize() {
  Serial.println(F("Initializing ConfigManager..."));

  // Initialize flash library
  if (!flash.begin()) {
    Serial.println(F("Error: Failed to initialize flash chip!"));
    return false;
  }
  Serial.print(F("Flash chip JEDEC ID: 0x"));
  Serial.println(flash.getJEDECID(), HEX);

  // Mount the filesystem
  if (!fatfs.begin(&flash)) {
    Serial.println(F("Error: Failed to mount filesystem!"));
    Serial.println(F("Ensure the flash chip is formatted using the SdFat_format example."));
    return false;
  }
  Serial.println(F("Filesystem mounted successfully."));

  // Load configuration
  if (!loadConfig()) {
    Serial.println(F("Config file not found or failed to load."));
    Serial.println(F("Creating default config..."));

    // Populate config struct from JSON
    DynamicJsonDocument doc(2048);  // Increased from 1024 to 2048

    DeserializationError error = deserializeJson(doc, defaultConfig);
    if (error) {
      Serial.print(F("Error parsing defaultConfig: "));
      Serial.println(error.c_str());
      return false;
    }

    // Assign values to config struct
    config.version = doc["version"] | 1;
    config.deviceName = doc["deviceName"] | "SkullOfFate";
    config.accel_calibration.x = doc["accel_calibration"]["x"] | 0;
    config.accel_calibration.y = doc["accel_calibration"]["y"] | 0;
    config.accel_calibration.z = doc["accel_calibration"]["z"] | 0;
    config.defaultAnimation = doc["defaultAnimation"] | 0;
    config.defaulti2cAddress = doc["defaulti2cAddress"] | 18;
    config.alternatei2cAddress1 = doc["alternatei2cAddress1"] | 19;
    config.alternatei2cAddress2 = doc["alternatei2cAddress2"] | 20;
    config.alternatei2cAddress3 = doc["alternatei2cAddress3"] | 21;  // Newly added field
    config.mic_calibration = doc["mic_calibration"] | 0;
    config.mag_calibration = doc["mag_calibration"] | 0;
    config.rfid_calibration = doc["rfid_calibration"] | 0;
    config.extra1 = doc["extra1"] | 0;
    config.extra2 = doc["extra2"] | 0;
    config.extra3 = doc["extra3"] | 0;
    config.extra4 = doc["extra4"] | 0;
    config.extra5 = doc["extra5"] | 0;
    config.neopixelmaxbrightness = doc["neopixelmaxbrightness"] | 255;  // Newly added field
    config.watchdogmaxtimeout = doc["watchdogmaxtimeout"] | 5000;        // Newly added field

    // Populate animation colors
    config.animation1_color = doc["animation1_color"] | 0;
    config.animation2_color = doc["animation2_color"] | 0;
    config.animation3_color = doc["animation3_color"] | 0;
    config.animation4_color = doc["animation4_color"] | 0;
    config.animation5_color = doc["animation5_color"] | 0;
    config.animation6_color = doc["animation6_color"] | 0;
    config.animation7_color = doc["animation7_color"] | 0;
    config.animation8_color = doc["animation8_color"] | 0;
    config.animation9_color = doc["animation9_color"] | 0;
    config.animation10_color = doc["animation10_color"] | 0;
    config.animation11_color = doc["animation11_color"] | 0;
    config.animation12_color = doc["animation12_color"] | 0;
    config.animation13_color = doc["animation13_color"] | 0;
    config.animation14_color = doc["animation14_color"] | 0;
    config.animation15_color = doc["animation15_color"] | 0;
    config.animation16_color = doc["animation16_color"] | 0;
    config.animation17_color = doc["animation17_color"] | 0;
    config.animation18_color = doc["animation18_color"] | 0;
    config.animation19_color = doc["animation19_color"] | 0;
    config.animation20_color = doc["animation20_color"] | 0;
    config.animation21_color = doc["animation21_color"] | 0;
    config.animation22_color = doc["animation22_color"] | 0;
    config.animation23_color = doc["animation23_color"] | 0;
    config.animation24_color = doc["animation24_color"] | 0;
    config.animation25_color = doc["animation25_color"] | 0;

    // Save the default configuration atomically
    if (saveConfigAtomic()) {
      Serial.println(F("Default configuration saved successfully."));
    } else {
      Serial.println(F("Error: Failed to save default configuration!"));
      return false;
    }
  } else {
    Serial.println(F("Configuration loaded successfully."));
    // printConfig();
  }

  return true;
}
bool ConfigManager::loadConfig() {
  if (!fatfs.exists(CONFIG_FILE)) {
    Serial.println(F("Config file does not exist."));
    return false;
  }

  File32 configFile = fatfs.open(CONFIG_FILE, FILE_READ);
  if (!configFile) {
    Serial.println(F("Failed to open config.json for reading."));
    return false;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  if (error) {
    Serial.print(F("JSON deserialization failed: "));
    Serial.println(error.c_str());
    return false;
  }

  // Update the config struct
  updateConfigStructFromDoc(doc);

  return true;
}


void ConfigManager::updateConfigStructFromDoc(const DynamicJsonDocument& doc) {
  config.version = doc["version"] | 1;
  config.deviceName = doc["deviceName"] | "SkullOfFate";
  config.accel_calibration.x = doc["accel_calibration"]["x"] | 0;
  config.accel_calibration.y = doc["accel_calibration"]["y"] | 0;
  config.accel_calibration.z = doc["accel_calibration"]["z"] | 0;
  config.defaultAnimation = doc["defaultAnimation"] | 0;
  config.defaulti2cAddress = doc["defaulti2cAddress"] | 18;
  config.alternatei2cAddress1 = doc["alternatei2cAddress1"] | 19;
  config.alternatei2cAddress2 = doc["alternatei2cAddress2"] | 20;
  config.alternatei2cAddress3 = doc["alternatei2cAddress3"] | 21;
  config.mic_calibration = doc["mic_calibration"] | 0;
  config.mag_calibration = doc["mag_calibration"] | 0;
  config.rfid_calibration = doc["rfid_calibration"] | 0;
  config.extra1 = doc["extra1"] | 0;
  config.extra2 = doc["extra2"] | 0;
  config.extra3 = doc["extra3"] | 0;
  config.extra4 = doc["extra4"] | 0;
  config.extra5 = doc["extra5"] | 0;
  config.neopixelmaxbrightness = doc["neopixelmaxbrightness"] | 255;
  config.watchdogmaxtimeout = doc["watchdogmaxtimeout"] | 5000;

  // Update animation colors
  config.animation1_color = doc["animation1_color"] | 0;
  config.animation2_color = doc["animation2_color"] | 0;
  config.animation3_color = doc["animation3_color"] | 0;
  config.animation4_color = doc["animation4_color"] | 0;
  config.animation5_color = doc["animation5_color"] | 0;
  config.animation6_color = doc["animation6_color"] | 0;
  config.animation7_color = doc["animation7_color"] | 0;
  config.animation8_color = doc["animation8_color"] | 0;
  config.animation9_color = doc["animation9_color"] | 0;
  config.animation10_color = doc["animation10_color"] | 0;
  config.animation11_color = doc["animation11_color"] | 0;
  config.animation12_color = doc["animation12_color"] | 0;
  config.animation13_color = doc["animation13_color"] | 0;
  config.animation14_color = doc["animation14_color"] | 0;
  config.animation15_color = doc["animation15_color"] | 0;
  config.animation16_color = doc["animation16_color"] | 0;
  config.animation17_color = doc["animation17_color"] | 0;
  config.animation18_color = doc["animation18_color"] | 0;
  config.animation19_color = doc["animation19_color"] | 0;
  config.animation20_color = doc["animation20_color"] | 0;
  config.animation21_color = doc["animation21_color"] | 0;
  config.animation22_color = doc["animation22_color"] | 0;
  config.animation23_color = doc["animation23_color"] | 0;
  config.animation24_color = doc["animation24_color"] | 0;
  config.animation25_color = doc["animation25_color"] | 0;

}


// Function to save configuration to config.json atomically
bool ConfigManager::saveConfigAtomic() {
  // Serial.println(F("Starting atomic save of config.json..."));

  // Step 1: Backup the existing config.json
  if (fatfs.exists(CONFIG_FILE)) {
    // Serial.println(F("Backing up existing config.json..."));
    if (!backupConfig()) {
      Serial.println(F("Error: Failed to backup existing config.json"));
      return false;
    }
    // Serial.println(F("Backup created successfully."));
  } else {
    Serial.println(F("No existing config.json found. Skipping backup."));
  }

  // Step 2: Write to a temporary file
  const char* tempFileName = "/config_temp.json";
  // Serial.print(F("Opening temporary file for writing: "));
  // Serial.println(tempFileName);
  File32 tempFile = fatfs.open(tempFileName, FILE_WRITE);
  if (!tempFile) {
    Serial.println(F("Error: Failed to open config_temp.json for writing."));
    return false;
  }
  // Serial.println(F("Temporary file opened successfully."));

  // Increase the DynamicJsonDocument size
  const size_t capacity = JSON_OBJECT_SIZE(29) + 1500;  // Adjusted for extended config
  DynamicJsonDocument doc(capacity);

  // Populate the JSON document with config data
  doc["version"] = config.version;
  doc["deviceName"] = config.deviceName;

  // Nested accel_calibration object
  JsonObject accel_cal = doc.createNestedObject("accel_calibration");
  accel_cal["x"] = config.accel_calibration.x;
  accel_cal["y"] = config.accel_calibration.y;
  accel_cal["z"] = config.accel_calibration.z;

  doc["defaultAnimation"] = config.defaultAnimation;
  doc["defaulti2cAddress"] = config.defaulti2cAddress;
  doc["alternatei2cAddress1"] = config.alternatei2cAddress1;
  doc["alternatei2cAddress2"] = config.alternatei2cAddress2;
  doc["alternatei2cAddress3"] = config.alternatei2cAddress3;      // Newly added field
  doc["mic_calibration"] = config.mic_calibration;
  doc["mag_calibration"] = config.mag_calibration;
  doc["rfid_calibration"] = config.rfid_calibration;
  doc["extra1"] = config.extra1;
  doc["extra2"] = config.extra2;
  doc["extra3"] = config.extra3;
  doc["extra4"] = config.extra4;
  doc["extra5"] = config.extra5;
  doc["neopixelmaxbrightness"] = config.neopixelmaxbrightness;    // Newly added field
  doc["watchdogmaxtimeout"] = config.watchdogmaxtimeout;          // Newly added field

  // Add animation colors
  doc["animation1_color"] = config.animation1_color;
  doc["animation2_color"] = config.animation2_color;
  doc["animation3_color"] = config.animation3_color;
  doc["animation4_color"] = config.animation4_color;
  doc["animation5_color"] = config.animation5_color;
  doc["animation6_color"] = config.animation6_color;
  doc["animation7_color"] = config.animation7_color;
  doc["animation8_color"] = config.animation8_color;
  doc["animation9_color"] = config.animation9_color;
  doc["animation10_color"] = config.animation10_color;
  doc["animation11_color"] = config.animation11_color;
  doc["animation12_color"] = config.animation12_color;
  doc["animation13_color"] = config.animation13_color;
  doc["animation14_color"] = config.animation14_color;
  doc["animation15_color"] = config.animation15_color;
  doc["animation16_color"] = config.animation16_color;
  doc["animation17_color"] = config.animation17_color;
  doc["animation18_color"] = config.animation18_color;
  doc["animation19_color"] = config.animation19_color;
  doc["animation20_color"] = config.animation20_color;
  doc["animation21_color"] = config.animation21_color;
  doc["animation22_color"] = config.animation22_color;
  doc["animation23_color"] = config.animation23_color;
  doc["animation24_color"] = config.animation24_color;
  doc["animation25_color"] = config.animation25_color;

  // Serialize JSON to the temp file
  // Serial.println(F("Serializing JSON to temporary file..."));
  if (serializeJsonPretty(doc, tempFile) == 0) {
    Serial.println(F("Error: Failed to write to config_temp.json"));
    tempFile.close();
    return false;
  }
  // Serial.println(F("Configuration written to temporary file successfully."));
  tempFile.close();

  // Step 3: Remove the existing config.json
  if (fatfs.exists(CONFIG_FILE)) {
    // Serial.println(F("Removing existing config.json..."));
    if (!fatfs.remove(CONFIG_FILE)) {
      Serial.println(F("Error: Failed to remove existing config.json"));
      return false;
    }
    // Serial.println(F("Existing config.json removed successfully."));
  } else {
    Serial.println(F("No existing config.json to remove."));
  }

  // Step 4: Rename temp file to config.json
  // Serial.print(F("Renaming "));
  // Serial.print(tempFileName);
  // Serial.print(F(" to "));
  // Serial.println(CONFIG_FILE);
  if (!fatfs.rename(tempFileName, CONFIG_FILE)) {
    Serial.println(F("Error: Failed to rename config_temp.json to config.json"));
    return false;
  }
  // Serial.println(F("Renamed config_temp.json to config.json successfully."));

  // Serial.println(F("Atomic save of config.json completed successfully."));
  return true;
}

// Function to backup the current config.json
bool ConfigManager::backupConfig() {
  const char* backupFileName = "/config_backup.json";

  // Serial.print(F("Creating backup file: "));
  // Serial.println(backupFileName);

  // Remove existing backup if it exists
  if (fatfs.exists(backupFileName)) {
    // Serial.println(F("Existing backup config_backup.json found. Removing..."));
    if (!fatfs.remove(backupFileName)) {
      Serial.println(F("Error: Failed to remove existing config_backup.json"));
      return false;
    }
    // Serial.println(F("Existing config_backup.json removed successfully."));
  }

  // Rename current config.json to config_backup.json
  // Serial.print(F("Renaming "));
  // Serial.print(CONFIG_FILE);
  // Serial.print(F(" to "));
  // Serial.println(backupFileName);
  if (!fatfs.rename(CONFIG_FILE, backupFileName)) {
    Serial.println(F("Error: Failed to backup config.json"));
    return false;
  }

  Serial.println(F("Backup of config.json created successfully."));
  return true;
}

// Function to print the current configuration to Serial
void ConfigManager::printConfig() {
  Serial.println(F("----- Current Configuration -----"));
  Serial.print(F("Version: "));
  Serial.println(config.version);
  Serial.print(F("Device Name: "));
  Serial.println(config.deviceName);
  Serial.print(F("Accel Calibration: x="));
  Serial.print(config.accel_calibration.x);
  Serial.print(F(", y="));
  Serial.print(config.accel_calibration.y);
  Serial.print(F(", z="));
  Serial.println(config.accel_calibration.z);
  Serial.print(F("Default Animation: "));
  Serial.println(config.defaultAnimation);
  Serial.print(F("Default I2C Address: 0x"));
  Serial.println(config.defaulti2cAddress, HEX);
  Serial.print(F("Alternate I2C Address 1: 0x"));
  Serial.println(config.alternatei2cAddress1, HEX);
  Serial.print(F("Alternate I2C Address 2: 0x"));
  Serial.println(config.alternatei2cAddress2, HEX);
  Serial.print(F("Alternate I2C Address 3: 0x"));
  Serial.println(config.alternatei2cAddress3, HEX);  // Newly added field
  Serial.print(F("Mic Calibration: "));
  Serial.println(config.mic_calibration);
  Serial.print(F("Mag Calibration: "));
  Serial.println(config.mag_calibration);
  Serial.print(F("RFID Calibration: "));
  Serial.println(config.rfid_calibration);
  Serial.print(F("Extra1: "));
  Serial.println(config.extra1);
  Serial.print(F("Extra2: "));
  Serial.println(config.extra2);
  Serial.print(F("Extra3: "));
  Serial.println(config.extra3);
  Serial.print(F("Extra4: "));
  Serial.println(config.extra4);
  Serial.print(F("Extra5: "));
  Serial.println(config.extra5);
  Serial.print(F("NeoPixel Max Brightness: "));
  Serial.println(config.neopixelmaxbrightness);  // Newly added field
  Serial.print(F("Watchdog Max Timeout: "));
  Serial.println(config.watchdogmaxtimeout);    // Newly added field

  // Print animation colors
  Serial.println(F("Animation Colors:"));
  Serial.print(F("  animation1_color: "));
  Serial.println(config.animation1_color);
  Serial.print(F("  animation2_color: "));
  Serial.println(config.animation2_color);
  Serial.print(F("  animation3_color: "));
  Serial.println(config.animation3_color);
  Serial.print(F("  animation4_color: "));
  Serial.println(config.animation4_color);
  Serial.print(F("  animation5_color: "));
  Serial.println(config.animation5_color);
  Serial.print(F("  animation6_color: "));
  Serial.println(config.animation6_color);
  Serial.print(F("  animation7_color: "));
  Serial.println(config.animation7_color);
  Serial.print(F("  animation8_color: "));
  Serial.println(config.animation8_color);
  Serial.print(F("  animation9_color: "));
  Serial.println(config.animation9_color);
  Serial.print(F("  animation10_color: "));
  Serial.println(config.animation10_color);
  Serial.print(F("  animation11_color: "));
  Serial.println(config.animation11_color);
  Serial.print(F("  animation12_color: "));
  Serial.println(config.animation12_color);
  Serial.print(F("  animation13_color: "));
  Serial.println(config.animation13_color);
  Serial.print(F("  animation14_color: "));
  Serial.println(config.animation14_color);
  Serial.print(F("  animation15_color: "));
  Serial.println(config.animation15_color);
  Serial.print(F("  animation16_color: "));
  Serial.println(config.animation16_color);
  Serial.print(F("  animation17_color: "));
  Serial.println(config.animation17_color);
  Serial.print(F("  animation18_color: "));
  Serial.println(config.animation18_color);
  Serial.print(F("  animation19_color: "));
  Serial.println(config.animation19_color);
  Serial.print(F("  animation20_color: "));
  Serial.println(config.animation20_color);
  Serial.print(F("  animation21_color: "));
  Serial.println(config.animation21_color);
  Serial.print(F("  animation22_color: "));
  Serial.println(config.animation22_color);
  Serial.print(F("  animation23_color: "));
  Serial.println(config.animation23_color);
  Serial.print(F("  animation24_color: "));
  Serial.println(config.animation24_color);
  Serial.print(F("  animation25_color: "));
  Serial.println(config.animation25_color);
  Serial.println(F("----------------------------------"));
}
bool ConfigManager::updateConfig(const String& key, const JsonVariant& value) {
  // Load existing config into doc
  if (!fatfs.exists(CONFIG_FILE)) {
    Serial.println(F("Error: config.json does not exist."));
    return false;
  }

  File32 configFile = fatfs.open(CONFIG_FILE, FILE_READ);
  if (!configFile) {
    Serial.println(F("Error: Failed to open config.json for reading."));
    return false;
  }

  DynamicJsonDocument doc(2048); // Adjust size as needed
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  if (error) {
    Serial.print(F("Error parsing config.json: "));
    Serial.println(error.c_str());
    return false;
  }

  // Update the specific key in doc
  if (doc.containsKey(key)) {
    doc[key] = value;
  } else {
    Serial.print(F("Error: Key '"));
    Serial.print(key);
    Serial.println(F("' does not exist in config."));
    return false;
  }

  // Update the config struct from the updated doc
  updateConfigStructFromDoc(doc);

  // Save the updated configuration
  if (!saveConfigAtomic()) {
    Serial.println(F("Error: Failed to save updated configuration."));
    return false;
  }

  Serial.println(F("Configuration updated successfully."));
  return true;
}
