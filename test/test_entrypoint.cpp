#include <unity.h>

#ifdef NATIVE_PLATFORM

// Forward declaration from test/native/src/test_maker_api.cpp.
// setUp()/tearDown() also live there since maker_api's tests share one
// MakerAPIModule/MockWebPlatformProvider fixture per test rather than
// constructing local instances (unlike sibling modules' per-test pattern).
void register_maker_api_tests();

int main(int argc, char **argv) {
  UNITY_BEGIN();

  register_maker_api_tests();

  UNITY_END();

  return 0;
}

// ESP32 entrypoint - on-device compilation/smoke-test verification
#else
#include <Arduino.h>

extern "C" void setUp(void) {}
extern "C" void tearDown(void) {}

// Forward declaration for on-device tests (defined under test/esp32)
void register_esp32_maker_api_tests();

void setup() {
  // Allow USB CDC/Serial to enumerate
  delay(2000);
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  UNITY_BEGIN();
  // Give the serial monitor a moment to attach before printing results
  delay(500);

  register_esp32_maker_api_tests();

  UNITY_END();
}

void loop() {
  // No-op
}
#endif
