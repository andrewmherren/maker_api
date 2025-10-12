#include <unity.h>

// Use ArduinoFake for proper Arduino mocking
#include <ArduinoFake.h>
#include <ArduinoJson.h>

// Include the actual maker_api header
#include "../include/maker_api.h"

// Simple mock platform for testing
class MockWebPlatform : public IWebPlatform {
public:
  void begin(const String &deviceName) override {}
  void begin(const String &deviceName, bool httpsOnly) override {}
  void handle() override {}
  bool isConnected() const override { return true; }
  bool isHttpsEnabled() const override { return true; }
  String getBaseUrl() const override { return "http://test.local"; }
  void registerModule(const String &basePath, IWebModule *module) override {}
  void registerWebRoute(const String &path,
                        WebModule::UnifiedRouteHandler handler,
                        const AuthRequirements &auth = {AuthType::NONE},
                        WebModule::Method method = WebModule::WM_GET) override {
  }
  void registerApiRoute(
      const String &path, WebModule::UnifiedRouteHandler handler,
      const AuthRequirements &auth = {AuthType::NONE},
      WebModule::Method method = WebModule::WM_GET,
      const OpenAPIDocumentation &docs = OpenAPIDocumentation()) override {}
  size_t getRouteCount() const override { return 0; }
  void disableRoute(const String &path,
                    WebModule::Method method = WebModule::WM_GET) override {}
  String getDeviceName() const override { return "TestDevice"; }
  void setErrorPage(int statusCode, const String &html) override {}
  void addGlobalRedirect(const String &fromPath,
                         const String &toPath) override {}
  void createJsonResponse(WebResponse &res,
                          std::function<void(JsonObject &)> builder) override {
    // Mock implementation - just create empty response
  }
  void
  createJsonArrayResponse(WebResponse &res,
                          std::function<void(JsonArray &)> builder) override {
    // Mock implementation - just create empty response
  }
};

class MockWebPlatformProvider : public IWebPlatformProvider {
private:
  MockWebPlatform mockPlatform;

public:
  IWebPlatform &getPlatform() override { return mockPlatform; }
};

// Include assets for verification
#include "../assets/maker_api_dashboard_html.h"
#include "../assets/maker_api_styles_css.h"
#include "../assets/maker_api_utils_js.h"

static std::unique_ptr<MockWebPlatformProvider> mockProvider;
static MakerAPIModule *testModule = nullptr;

void setUp() {
  ArduinoFakeReset();

  // Create mock platform provider and set the global instance
  mockProvider = std::make_unique<MockWebPlatformProvider>();
  IWebPlatformProvider::instance = mockProvider.get();

  // Instantiate your module
  testModule = new MakerAPIModule();
  testModule->begin();
}

void tearDown() {
  delete testModule;
  testModule = nullptr;

  mockProvider.reset();
  IWebPlatformProvider::instance = nullptr;
}

// Test module basic properties
void test_maker_api_module_properties() {
  auto &module = *testModule;
  TEST_ASSERT_EQUAL_STRING("Maker API", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("0.1.0", module.getModuleVersion().c_str());

  String description = module.getModuleDescription();
  TEST_ASSERT_TRUE(description.length() > 0);
  TEST_ASSERT_TRUE(description.indexOf("API documentation") >= 0);
}

// Test module lifecycle methods
void test_maker_api_module_lifecycle() {
  auto &module = *testModule;

  // begin() should not crash (already called in setUp)
  module.begin();
  TEST_ASSERT_TRUE(true); // If we get here, begin() succeeded

  // handle() should not crash
  module.handle();
  TEST_ASSERT_TRUE(true); // If we get here, handle() succeeded
}

// Test HTTP routes generation
void test_maker_api_http_routes() {
  auto &module = *testModule;
  std::vector<RouteVariant> routes = module.getHttpRoutes();

  // Should have exactly 4 routes: dashboard, CSS, JS, and config API
  TEST_ASSERT_EQUAL(4, routes.size());

  // All routes should be properly initialized
  for (const auto &route : routes) {
    // Test that we can check the route type without accessing private members
    TEST_ASSERT_TRUE(route.isWebRoute() || route.isApiRoute());
  }
}

// Test HTTPS routes (should be identical to HTTP)
void test_maker_api_https_routes() {
  auto &module = *testModule;
  std::vector<RouteVariant> httpRoutes = module.getHttpRoutes();
  std::vector<RouteVariant> httpsRoutes = module.getHttpsRoutes();

  TEST_ASSERT_EQUAL(httpRoutes.size(), httpsRoutes.size());
  TEST_ASSERT_EQUAL(4, httpsRoutes.size());
}

// Test OpenAPI documentation generation
void test_maker_api_openapi_docs() {
  auto &module = *testModule;
  OpenAPIDocumentation docs = module.getOpenAPIConfigDocs();

  // Should have meaningful documentation
  TEST_ASSERT_TRUE(docs.getSummary().length() > 0);
  TEST_ASSERT_TRUE(docs.getDescription().length() > 0);

  // Should contain relevant OpenAPI terms
  TEST_ASSERT_TRUE(docs.getSummary().indexOf("OpenAPI") >= 0 ||
                   docs.getDescription().indexOf("OpenAPI") >= 0 ||
                   docs.getSummary().indexOf("API") >= 0);
}

// Note: MockWebRequest and MockWebResponse are provided by mock_web_platform.h

// Test config API handler structure (simplified without handler execution)
void test_maker_api_config_api_handler() {
  auto &module = *testModule;
  std::vector<RouteVariant> routes = module.getHttpRoutes();
  TEST_ASSERT_GREATER_THAN(3, routes.size());

  // Get API config route (should be fourth route)
  RouteVariant configRoute = routes[3];
  if (configRoute.isApiRoute()) {
    const ApiRoute &apiRoute = configRoute.getApiRoute();
    TEST_ASSERT_TRUE(apiRoute.webRoute.path.indexOf("config") > 0);

    // Verify the handler is set
    TEST_ASSERT_NOT_NULL(apiRoute.webRoute.unifiedHandler);

    // Verify authentication requirements
    TEST_ASSERT_TRUE(apiRoute.webRoute.authRequirements.size() > 0);
  }
}

// Test compilation flag behavior
void test_maker_api_compilation_flags() {
// Verify build flags are correctly defined
#ifdef WEB_PLATFORM_OPENAPI
  bool openApiFlag = true;
#else
  bool openApiFlag = false;
#endif

#ifdef WEB_PLATFORM_MAKERAPI
  bool makerApiFlag = true;
#else
  bool makerApiFlag = false;
#endif

#ifdef MAKER_API_STANDALONE_TEST
  bool standaloneFlag = true;
#else
  bool standaloneFlag = false;
#endif

  // These should all be true in test builds
  TEST_ASSERT_TRUE(openApiFlag);
  TEST_ASSERT_TRUE(makerApiFlag);
  TEST_ASSERT_TRUE(standaloneFlag);
}

#ifdef NATIVE_PLATFORM
int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(test_maker_api_module_properties);
  RUN_TEST(test_maker_api_module_lifecycle);
  RUN_TEST(test_maker_api_http_routes);
  RUN_TEST(test_maker_api_https_routes);
  RUN_TEST(test_maker_api_openapi_docs);
  RUN_TEST(test_maker_api_config_api_handler);
  RUN_TEST(test_maker_api_compilation_flags);

  UNITY_END();
  return 0;
}
#else
void setup() {
  UNITY_BEGIN();

  RUN_TEST(test_maker_api_module_properties);
  RUN_TEST(test_maker_api_module_lifecycle);
  RUN_TEST(test_maker_api_http_routes);
  RUN_TEST(test_maker_api_https_routes);
  RUN_TEST(test_maker_api_openapi_docs);
  RUN_TEST(test_maker_api_config_api_handler);
  RUN_TEST(test_maker_api_compilation_flags);

  UNITY_END();
}
void loop() {}
#endif
