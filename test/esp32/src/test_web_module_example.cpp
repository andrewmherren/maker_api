#include <unity.h>
#include <Arduino.h>

#ifndef NATIVE_PLATFORM

#include <maker_api.h>
#include <testing/production_platform_provider.h>
#include <vector>

// ===========================================================================
// ESP32 Hardware Tests
// ===========================================================================
// These tests verify that the module compiles and runs on actual ESP32 hardware
// They are intentionally simple - complex logic should be tested in native tests

void test_esp32_module_compiles() {
  // Just verify the module can be instantiated
  ProductionPlatformProvider provider;
  MakerAPIModule module(provider);
  TEST_ASSERT_TRUE(true);
}

void test_esp32_begin_does_not_crash() {
  ProductionPlatformProvider provider;
  MakerAPIModule module(provider);
  module.begin();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_handle_does_not_crash() {
  ProductionPlatformProvider provider;
  MakerAPIModule module(provider);
  module.begin();
  module.handle();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_get_routes_does_not_crash() {
  ProductionPlatformProvider provider;
  MakerAPIModule module(provider);
  std::vector<RouteVariant> httpRoutes = module.getHttpRoutes();
  std::vector<RouteVariant> httpsRoutes = module.getHttpsRoutes();
  TEST_ASSERT_TRUE(httpRoutes.size() > 0);
  TEST_ASSERT_TRUE(httpsRoutes.size() > 0);
}

void test_esp32_module_metadata() {
  ProductionPlatformProvider provider;
  MakerAPIModule module(provider);
  
  String name = module.getModuleName();
  String version = module.getModuleVersion();
  String description = module.getModuleDescription();
  
  TEST_ASSERT_TRUE(name.length() > 0);
  TEST_ASSERT_TRUE(version.length() > 0);
  TEST_ASSERT_TRUE(description.length() > 0);
}

#endif // NATIVE_PLATFORM
