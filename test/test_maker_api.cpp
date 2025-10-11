#include <unity.h>

// Use ArduinoFake for proper Arduino mocking
#include <ArduinoFake.h>
#include <ArduinoJson.h>

// Include mocks for web platform with compatibility layers
#include <testing/mock_web_platform.h>
#include <testing/utils/string_compat.h>

// Include the actual maker_api header
#include "../include/maker_api.h"

// Include the test setup helper
#include <testing/testing_platform_provider.h>
#include <testing/unified_test_setup.h>

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
  auto &module = auto &module = *testModule;
  ;
  TEST_ASSERT_EQUAL_STRING("Maker API", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("0.1.0", module.getModuleVersion().c_str());

  String description = module.getModuleDescription();
  TEST_ASSERT_TRUE(description.length() > 0);
  TEST_ASSERT_TRUE(description.indexOf("API documentation") >= 0);
}

// Test module lifecycle methods
void test_maker_api_module_lifecycle() {
  auto &module = auto &module = *testModule;
  ;

  // begin() should not crash (already called in setUp)
  module.begin();
  TEST_ASSERT_TRUE(true); // If we get here, begin() succeeded

  // handle() should not crash
  module.handle();
  TEST_ASSERT_TRUE(true); // If we get here, handle() succeeded
}

// Test HTTP routes generation
void test_maker_api_http_routes() {
  auto &module = auto &module = *testModule;
  ;
  std::vector<RouteVariant> routes = module.getHttpRoutes();

  // Should have exactly 4 routes: dashboard, CSS, JS, and config API
  TEST_ASSERT_EQUAL(4, routes.size());

  // All routes should be properly initialized
  for (const auto &route : routes) {
    TEST_ASSERT_NOT_NULL(route.route);
  }
}

// Test HTTPS routes (should be identical to HTTP)
void test_maker_api_https_routes() {
  auto &module = auto &module = *testModule;
  ;
  std::vector<RouteVariant> httpRoutes = module.getHttpRoutes();
  std::vector<RouteVariant> httpsRoutes = module.getHttpsRoutes();

  TEST_ASSERT_EQUAL(httpRoutes.size(), httpsRoutes.size());
  TEST_ASSERT_EQUAL(4, httpsRoutes.size());
}

// Test OpenAPI documentation generation
void test_maker_api_openapi_docs() {
  auto &module = auto &module = *testModule;
  ;
  OpenAPIDocumentation docs = module.getOpenAPIConfigDocs();

  // Should have meaningful documentation
  TEST_ASSERT_TRUE(docs.summary.length() > 0);
  TEST_ASSERT_TRUE(docs.description.length() > 0);

  // Should contain relevant OpenAPI terms
  TEST_ASSERT_TRUE(docs.summary.indexOf("OpenAPI") >= 0 ||
                   docs.description.indexOf("OpenAPI") >= 0 ||
                   docs.summary.indexOf("API") >= 0);
}

// Mock request/response classes compatible with ArduinoFake
class MockWebRequest : public WebRequest {
private:
  std::map<std::string, std::string> params;
  std::string body;
  AuthContext authCtx;

public:
  MockWebRequest() : authCtx(false, "") {}

  void setParam(const std::string &name, const std::string &value) {
    params[name] = value;
  }

  void setBody(const std::string &b) { body = b; }
  void setAuthContext(const AuthContext &ctx) { authCtx = ctx; }

  String getParam(const String &name) override {
    std::string stdName = name.c_str();
    return params.count(stdName) ? String(params[stdName].c_str()) : String("");
  }

  String getBody() override { return String(body.c_str()); }
  const AuthContext &getAuthContext() const override { return authCtx; }
};

class MockWebResponse : public WebResponse {
public:
  String content;
  String contentType;
  int statusCode = 200;
  std::map<std::string, std::string> headers;

  void setContent(const String &c, const String &ct) override {
    content = c;
    contentType = ct;
  }

  void setProgmemContent(const char *c, const String &ct) override {
    content = String(c); // In mock, just convert to String
    contentType = ct;
  }

  void setStatus(int code) override { statusCode = code; }

  void setHeader(const String &name, const String &value) override {
    headers[std::string(name.c_str())] = std::string(value.c_str());
  }
};

// Test config API handler with proper JSON handling
void test_maker_api_config_api_handler() {
  auto &module = auto &module = *testModule;
  ;
  std::vector<RouteVariant> routes = module.getHttpRoutes();
  TEST_ASSERT_GREATER_THAN(3, routes.size());

  // Get API config route (should be fourth route)
  RouteVariant configRoute = routes[3];
  if (configRoute.type == RouteVariant::API_ROUTE) {
    ApiRoute *apiRoute = static_cast<ApiRoute *>(configRoute.route);
    TEST_ASSERT_TRUE(apiRoute->path.indexOf("config") > 0);

    MockWebRequest req;
    MockWebResponse res;
    req.setAuthContext(AuthContext(true, "testuser"));

    if (apiRoute->handler) {
      apiRoute->handler(req, res);

      TEST_ASSERT_EQUAL(200, res.statusCode);
      TEST_ASSERT_EQUAL_STRING("application/json", res.contentType.c_str());
      TEST_ASSERT_TRUE(res.content.length() > 0);

      // Parse and validate JSON response using std::string for compatibility
      std::string jsonStr = toStdString(res.content);
      DynamicJsonDocument doc(512);
      DeserializationError error =
          StringCompat::deserializeJsonFromStdString(doc, jsonStr);
      TEST_ASSERT_TRUE(error == DeserializationError::Ok);

      // Should have success field
      TEST_ASSERT_TRUE(doc["success"].as<bool>());

      // Should have OpenApiConfig object
      TEST_ASSERT_TRUE(doc["OpenApiConfig"].is<JsonObject>());

// Check flag values based on compilation flags
#ifdef WEB_PLATFORM_OPENAPI
      TEST_ASSERT_TRUE(doc["OpenApiConfig"]["fullSpec"].as<bool>());
#else
      TEST_ASSERT_FALSE(doc["OpenApiConfig"]["fullSpec"].as<bool>());
#endif

#ifdef WEB_PLATFORM_MAKERAPI
      TEST_ASSERT_TRUE(doc["OpenApiConfig"]["makerSpec"].as<bool>());
#else
      TEST_ASSERT_FALSE(doc["OpenApiConfig"]["makerSpec"].as<bool>());
#endif
    }
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
