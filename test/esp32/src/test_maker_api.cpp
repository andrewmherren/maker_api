#include <unity.h>
#include <Arduino.h>

#ifndef NATIVE_PLATFORM

#include <maker_api.h>
#include <vector>

// ===========================================================================
// ESP32 Hardware Tests
// ===========================================================================
// These tests verify that the module compiles and runs on actual ESP32 hardware
// They are intentionally simple - complex logic should be tested in native tests

void test_esp32_module_compiles() {
  // Module uses global provider set in setup
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
  module.handle();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_routes_not_empty() {
  MakerAPIModule module;
  auto routes = module.getHttpRoutes();
  TEST_ASSERT_TRUE(routes.size() > 0);
}

void test_esp32_version_matches_macro() {
  MakerAPIModule module;
  TEST_ASSERT_EQUAL_STRING(WEB_MODULE_VERSION_STR, module.getModuleVersion().c_str());
}

// ===========================================================================
// Test Registration
// ===========================================================================

void register_esp32_maker_api_tests() {
  RUN_TEST(test_esp32_module_compiles);
  RUN_TEST(test_esp32_begin_does_not_crash);
  RUN_TEST(test_esp32_handle_does_not_crash);
  RUN_TEST(test_esp32_routes_not_empty);
  RUN_TEST(test_esp32_version_matches_macro);
}

#endif // !NATIVE_PLATFORM
