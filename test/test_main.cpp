#include "unity_config.h"
#include <unity.h>

// Use ArduinoFake for proper Arduino mocking
#include <ArduinoFake.h>
#include <ArduinoJson.h>

// Include all test header files
#include "test_maker_api.h"

// Use centralized testing infrastructure from web_platform_interface
#include <testing/testing_platform_provider.h>

// Include the actual maker_api header
#include "../include/maker_api.h"

// Global test variables that are shared with test_maker_api.cpp
std::unique_ptr<MockWebPlatformProvider> mockProvider;
std::unique_ptr<MakerAPIModule> testModule;

void setUp() {
  ArduinoFakeReset();

  // Create mock platform provider and set the global instance
  if (!mockProvider) {
    mockProvider.reset(new MockWebPlatformProvider());
  }
  IWebPlatformProvider::instance = mockProvider.get();

  // Instantiate your module using default constructor
  if (!testModule) {
    testModule.reset(new MakerAPIModule());
  }
  testModule->begin();
}

void tearDown() {
  if (testModule) {
    testModule->begin(); // Ensure module is in clean state
    testModule.reset(nullptr);
  }

  IWebPlatformProvider::instance = nullptr;

  if (mockProvider) {
    mockProvider.reset(nullptr);
  }
}
#ifdef NATIVE_PLATFORM
int main(int argc, char **argv) {
  UnityBegin(__FILE__);

  // Register and run all test groups
  register_maker_api_tests();

  return UnityEnd();
}
#else
void setup() {
  UnityBegin(__FILE__);

  // Register and run all test groups
  register_maker_api_tests();

  UnityEnd();
}

/* Empty loop function - required for Arduino framework but not used in tests */
void loop() {}
#endif