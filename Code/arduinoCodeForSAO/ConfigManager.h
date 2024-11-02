// ConfigManager.h
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <SdFat.h>
#include <ArduinoJson.h>
#include <Adafruit_SPIFlash.h>

// Configuration file path
#define CONFIG_FILE "/config.json"

// Configuration structure
struct Config {
  int version;
  String deviceName;
  struct {
    int x;
    int y;
    int z;
  } accel_calibration;
  int defaultAnimation;
  int defaulti2cAddress;
  int alternatei2cAddress1;
  int alternatei2cAddress2;
  int alternatei2cAddress3;          // Newly added field
  int mic_calibration;
  int mag_calibration;
  int rfid_calibration;
  int extra1;
  int extra2;
  int extra3;
  int extra4;
  int extra5;
  int neopixelmaxbrightness;          // Newly added field
  int watchdogmaxtimeout;            // Newly added field
  // Animation colors
  int animation1_color;
  int animation2_color;
  int animation3_color;
  int animation4_color;
  int animation5_color;
  int animation6_color;
  int animation7_color;
  int animation8_color;
  int animation9_color;
  int animation10_color;
  int animation11_color;
  int animation12_color;
  int animation13_color;
  int animation14_color;
  int animation15_color;
  int animation16_color;
  int animation17_color;
  int animation18_color;
  int animation19_color;
  int animation20_color;
  int animation21_color;
  int animation22_color;
  int animation23_color;
  int animation24_color;
  int animation25_color;
};




// ConfigManager class
class ConfigManager {
private:
  void updateConfigStructFromDoc(const DynamicJsonDocument& doc);
public:
  // Constructor accepts a reference to an Adafruit_SPIFlash object
  ConfigManager(Adafruit_SPIFlash& flashRef);
  
  // Initializes flash and filesystem, loads or creates config
  bool initialize();
  
  // Loads configuration from config.json
  bool loadConfig();
  
  // Saves configuration atomically to prevent corruption
  bool saveConfigAtomic();
  
  // Saves a backup of the current config.json
  bool backupConfig();
  
  // Prints current configuration to Serial Monitor
  void printConfig();
  
  // Updates specific configuration parameters
  bool updateConfig(const String& key, const JsonVariant& value);
  
  // Getter for config
  Config getConfig() const { return config; }
  
private:
  Config config;
  Adafruit_SPIFlash& flash;  // Reference to the flash object
  FatVolume fatfs;

  // Default configuration parameters as a JSON string
  const char* defaultConfig = R"json(
    {
      "version": 1,
      "deviceName": "SkullOfFate",
      "accel_calibration": {
        "x": 0,
        "y": 0,
        "z": 0
      },
      "defaultAnimation": 0,
      "defaulti2cAddress": 19,
      "alternatei2cAddress1": 19,
      "alternatei2cAddress2": 20,
      "alternatei2cAddress3": 21,
      "mic_calibration": 0,
      "mag_calibration": 0,
      "rfid_calibration": 0,
      "extra1": 0,
      "extra2": 0,
      "extra3": 0,
      "extra4": 0,
      "extra5": 0,
      "neopixelmaxbrightness": 10,
      "watchdogmaxtimeout": 8000,
      "animation1_color": 0,
      "animation2_color": 0,
      "animation3_color": 0,
      "animation4_color": 0,
      "animation5_color": 0,
      "animation6_color": 0,
      "animation7_color": 0,
      "animation8_color": 0,
      "animation9_color": 0,
      "animation10_color": 0,
      "animation11_color": 0,
      "animation12_color": 0,
      "animation13_color": 0,
      "animation14_color": 0,
      "animation15_color": 0,
      "animation16_color": 0,
      "animation17_color": 0,
      "animation18_color": 0,
      "animation19_color": 0,
      "animation20_color": 0,
      "animation21_color": 0,
      "animation22_color": 0,
      "animation23_color": 0,
      "animation24_color": 0,
      "animation25_color": 0
    }
  )json";
};

// Declare the ConfigManager instance as extern
extern ConfigManager configManager;

#endif // CONFIG_MANAGER_H
