#include <Arduino.h>
#include <unity.h>

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

  // Every TEST_ASSERT_EQUAL* numeric-comparison variant crashes inside
  // ESP-IDF's bundled Unity's own internal print path on this toolchain,
  // even when the values are equal - confirmed via a deliberate
  // encoded-fault-address crash dump (EXCVADDR decoded to
  // httpRoutes.size()=4, httpsRoutes.size()=4, both correct - this is not
  // a real bug in getHttpRoutes()/getHttpsRoutes()). TEST_ASSERT_TRUE takes
  // Unity's boolean-assertion path instead, which doesn't hit this.
  TEST_ASSERT_TRUE(httpRoutes.size() == 4);
  TEST_ASSERT_TRUE(httpRoutes.size() == httpsRoutes.size());
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
