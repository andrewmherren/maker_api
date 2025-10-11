#include "maker_api.h"
#include <ArduinoJson.h>

#ifndef MAKER_API_STANDALONE_TEST
#include "platform_provider.h"
#endif

// Include static assets
#include "../assets/maker_api_dashboard_html.h"
#include "../assets/maker_api_styles_css.h"
#include "../assets/maker_api_utils_js.h"

// Global instance of MakerAPIModule
MakerAPIModule makerAPI;

MakerAPIModule::MakerAPIModule() : platformProvider(nullptr) {
#ifndef MAKER_API_STANDALONE_TEST
  // In production, use the global platform provider
  platformProvider = IWebPlatformProvider::instance;
#endif
}

MakerAPIModule::MakerAPIModule(IWebPlatformProvider* provider) : platformProvider(provider) {}

MakerAPIModule::~MakerAPIModule() = default;

void MakerAPIModule::begin() {
  // Nothing needed here - we're just serving UI assets
}

void MakerAPIModule::handle() {
  // Nothing to do in regular processing
}

OpenAPIDocumentation MakerAPIModule::getOpenAPIConfigDocs() {
  return OpenAPIFactory::create("Get OpenAPI configuration",
                                "Retrieves a system information about the "
                                "availability of OpenAPI information.",
                                "getOpenAPIConfig", {"Maker API"})
      .withResponseExample(R"({
        "success": true,
        "OpenApiConfig": {
          fullSpec: true
          makerSpec: true
        }
      })")
      .withResponseSchema(OpenAPIFactory::createSuccessResponse(
          "System OpenAPI configuration"));
}

void MakerAPIModule::getOpenAPIConfigHandler(WebRequest &req,
                                             WebResponse &res) {

  bool fullSpec = false;
  bool makerSpec = false;

#if OPENAPI_ENABLED
  fullSpec = true;
#endif
#if MAKERAPI_ENABLED
  makerSpec = true;
#endif

  // Use JsonResponseBuilder for simple response
  getPlatform().createJsonResponse(res, [&](JsonObject &root) {
    root["success"] = true;

    JsonObject config = root["OpenApiConfig"].to<JsonObject>();
    config["fullSpec"] = fullSpec;
    config["makerSpec"] = makerSpec;
  });
}

std::vector<RouteVariant> MakerAPIModule::getHttpRoutes() {
  std::vector<RouteVariant> routes;

  // Main dashboard routes
  routes.push_back(WebRoute("/", WebModule::WM_GET,
                            [this](WebRequest &req, WebResponse &res) {
                              res.setProgmemContent(MAKER_API_DASHBOARD_HTML,
                                                    "text/html");
                            },
                            {AuthType::NONE}));

  // Static assets
  routes.push_back(
      WebRoute("/assets/maker-api-style.css", WebModule::WM_GET,
               [this](WebRequest &req, WebResponse &res) {
                 res.setProgmemContent(MAKER_API_STYLES_CSS, "text/css");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::NONE}));

  routes.push_back(
      WebRoute("/assets/maker-api-utils.js", WebModule::WM_GET,
               [this](WebRequest &req, WebResponse &res) {
                 res.setProgmemContent(MAKER_API_UTILS_JS,
                                       "application/javascript; charset=utf-8");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::NONE}));

  routes.push_back(ApiRoute(
      "/config", WebModule::WM_POST,
      [this](WebRequest &req, WebResponse &res) {
        getOpenAPIConfigHandler(req, res);
      },
      {AuthType::SESSION, AuthType::PAGE_TOKEN, AuthType::TOKEN},
      API_DOC_BLOCK(getOpenAPIConfigDocs())));

  return routes;
}

std::vector<RouteVariant> MakerAPIModule::getHttpsRoutes() {
  return getHttpRoutes();
}