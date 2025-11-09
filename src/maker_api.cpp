#include "maker_api.h"

#ifndef STANDALONE_TESTS
#include "platform_provider.h"
#endif

// Include static assets
#include "../assets/maker_api_dashboard_html.h"
#include "../assets/maker_api_styles_css.h"
#include "../assets/maker_api_utils_js.h"

// Global instance of MakerAPIModule for production builds
#if defined(ARDUINO) || defined(ESP_PLATFORM)
// NOSONAR - This module instance must be mutable as it maintains state and
// implements lifecycle methods
MakerAPIModule makerAPI; // NOSONAR
#endif

// ===========================================================================
// Constructor & Lifecycle
// ===========================================================================

MakerAPIModule::MakerAPIModule() : platformProvider(nullptr) {
#ifndef STANDALONE_TESTS
  // In production, use the global platform provider
  platformProvider = IWebPlatformProvider::instance;
#endif
}

MakerAPIModule::MakerAPIModule(IWebPlatformProvider *provider)
    : platformProvider(provider) {}

MakerAPIModule::~MakerAPIModule() = default;

void MakerAPIModule::begin() {
  DEBUG_PRINTLN("Maker API module initialized");
}

void MakerAPIModule::handle() {
  // Nothing to do in regular processing
}

// ===========================================================================
// OpenAPI Configuration Helpers
// ===========================================================================

MakerAPIModule::OpenAPIConfigStatus MakerAPIModule::getOpenAPIStatus() const {
  OpenAPIConfigStatus status;
  status.fullSpec = false;
  status.makerSpec = false;

#if OPENAPI_ENABLED
  status.fullSpec = true;
#endif
#if MAKERAPI_ENABLED
  status.makerSpec = true;
#endif

  return status;
}

OpenAPIDocumentation MakerAPIModule::getOpenAPIConfigDocs() const {
  return OpenAPIFactory::create("Get OpenAPI configuration",
                                "Retrieves system information about the "
                                "availability of OpenAPI documentation.",
                                "getOpenAPIConfig", {"Maker API"})
      .withResponseExample(R"({
        "success": true,
        "OpenApiConfig": {
          "fullSpec": true,
          "makerSpec": true
        }
      })")
      .withResponseSchema(OpenAPIFactory::createSuccessResponse(
          "System OpenAPI configuration"));
}

void MakerAPIModule::getOpenAPIConfigHandler(RequestT &req, ResponseT &res) {
  OpenAPIConfigStatus status = getOpenAPIStatus();

  respondJson(res, [&status](JsonObject &root) {
    root["success"] = true;

    JsonObject config = root["OpenApiConfig"].to<JsonObject>();
    config["fullSpec"] = status.fullSpec;
    config["makerSpec"] = status.makerSpec;
  });
}

// ===========================================================================
// Route Registration
// ===========================================================================

std::vector<RouteVariant> MakerAPIModule::getHttpRoutes() {
  return {
      // Main dashboard page
      WebRoute("/", WebModule::WM_GET,
               [](RequestT &req, ResponseT &res) {
                 res.setProgmemContent(MAKER_API_DASHBOARD_HTML, "text/html");
               },
               {AuthType::NONE}),

      // CSS stylesheet
      WebRoute("/assets/maker-api-style.css", WebModule::WM_GET,
               [](RequestT &req, ResponseT &res) {
                 res.setProgmemContent(MAKER_API_STYLES_CSS, "text/css");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::NONE}),

      // JavaScript utilities
      WebRoute("/assets/maker-api-utils.js", WebModule::WM_GET,
               [](RequestT &req, ResponseT &res) {
                 res.setProgmemContent(MAKER_API_UTILS_JS,
                                       "application/javascript; charset=utf-8");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::NONE}),

      // OpenAPI configuration endpoint
      ApiRoute("/config", WebModule::WM_POST,
               [this](RequestT &req, ResponseT &res) {
                 getOpenAPIConfigHandler(req, res);
               },
               {AuthType::SESSION, AuthType::PAGE_TOKEN, AuthType::TOKEN},
               API_DOC_BLOCK(getOpenAPIConfigDocs()))};
}

std::vector<RouteVariant> MakerAPIModule::getHttpsRoutes() {
  return getHttpRoutes();
}