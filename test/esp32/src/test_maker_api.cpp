#include <unity.h>
#include <Arduino.h>

#ifndef NATIVE_PLATFORM

#include <maker_api.h>

// ===========================================================================
// ESP32 Hardware Tests
// ===========================================================================
// These verify the module compiles and runs on actual ESP32 hardware.
// MAKER_API_STANDALONE_TEST (set by test_base) keeps the default constructor
// from touching a global IWebPlatformProvider, so these stay hardware-tolerant
// smoke tests - no platform-backed handler invocation, no mocks.

void test_esp32_module_compiles() {
  MakerAPIModule module;
  TEST_ASSERT_TRUE(true);
}

void test_esp32_begin_does_not_crash() {
  MakerAPIModule module;
  module.begin();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_handle_does_not_crash() {
  MakerAPIModule module;
  module.begin();
  module.handle();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_get_routes_does_not_crash() {
  MakerAPIModule module;

  auto httpRoutes = module.getHttpRoutes();
  auto httpsRoutes = module.getHttpsRoutes();

  TEST_ASSERT_EQUAL(4, httpRoutes.size());
  TEST_ASSERT_EQUAL(httpRoutes.size(), httpsRoutes.size());
}

void test_esp32_module_metadata() {
  MakerAPIModule module;

  String name = module.getModuleName();
  String version = module.getModuleVersion();
  String description = module.getModuleDescription();

  TEST_ASSERT_TRUE(name.length() > 0);
  TEST_ASSERT_EQUAL_STRING(WEB_MODULE_VERSION_STR, version.c_str());
  TEST_ASSERT_TRUE(description.length() > 0);
}

void register_esp32_maker_api_tests() {
  RUN_TEST(test_esp32_module_compiles);
  RUN_TEST(test_esp32_begin_does_not_crash);
  RUN_TEST(test_esp32_handle_does_not_crash);
  RUN_TEST(test_esp32_get_routes_does_not_crash);
  RUN_TEST(test_esp32_module_metadata);
}

#endif // !NATIVE_PLATFORM
