#include <unity.h>

#ifdef NATIVE_PLATFORM
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <testing/testing_platform_provider.h>
#include <maker_api.h>
using namespace fakeit;

// ===========================================================================
// Module Metadata Tests
// ===========================================================================

static void test_module_metadata() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  TEST_ASSERT_EQUAL_STRING("Maker API", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("0.1.0", module.getModuleVersion().c_str());
  TEST_ASSERT_TRUE_MESSAGE(module.getModuleDescription().length() > 0,
                           "Description should be non-empty");
}

// ===========================================================================
// Configuration Tests
// ===========================================================================

static void test_parseConfig_minimal() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  JsonVariant nullConfig;
  module.parseConfig(nullConfig);
  TEST_ASSERT_TRUE(true); // Should complete without error
}

// ===========================================================================
// Lifecycle Tests
// ===========================================================================

static void test_begin_calls_initialization() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  module.begin();
  TEST_ASSERT_TRUE(true);
}

static void test_handle_completes() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  module.handle();
  TEST_ASSERT_TRUE(true);
}

// ===========================================================================
// Route Registration Tests
// ===========================================================================

static void test_routes_built_and_sizes() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  auto http = module.getHttpRoutes();
  auto https = module.getHttpsRoutes();
  TEST_ASSERT_EQUAL(4, http.size()); // dashboard + css + js + config
  TEST_ASSERT_EQUAL(http.size(), https.size());
}

static void test_http_routes_structure() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  auto routes = module.getHttpRoutes();
  for (const auto &route : routes) {
    TEST_ASSERT_TRUE(route.isWebRoute() || route.isApiRoute());
  }
}

// ===========================================================================
// Route Handler Tests
// ===========================================================================

static void test_getOpenAPIConfigHandler_returns_json() {
  TestingPlatformProvider provider;
  MakerAPIModule module(provider);
  WebRequestCore req;
  WebResponseCore res;
  module.getOpenAPIConfigHandler(req, res);
  TEST_ASSERT_EQUAL_STRING("application/json", res.getMimeType().c_str());
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, res.getContent());
  TEST_ASSERT_FALSE_MESSAGE(err, "JSON parse error");
  TEST_ASSERT_TRUE(doc.containsKey("success"));
  TEST_ASSERT_TRUE(doc.containsKey("OpenApiConfig"));
  JsonObject config = doc["OpenApiConfig"];
  TEST_ASSERT_TRUE(config.containsKey("fullSpec"));
  TEST_ASSERT_TRUE(config.containsKey("makerSpec"));
}

// ===========================================================================
// Test Registration
// ===========================================================================

void register_maker_api_tests() {
  RUN_TEST(test_module_metadata);
  RUN_TEST(test_parseConfig_minimal);
  RUN_TEST(test_begin_calls_initialization);
  RUN_TEST(test_handle_completes);
  RUN_TEST(test_routes_built_and_sizes);
  RUN_TEST(test_http_routes_structure);
  RUN_TEST(test_getOpenAPIConfigHandler_returns_json);
}

#endif // NATIVE_PLATFORM
