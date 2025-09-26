#include "maker_api.h"
#include <ArduinoJson.h>

// Include static assets
#include "../assets/maker_api_dashboard_html.h"
#include "../assets/maker_api_styles_css.h"
#include "../assets/maker_api_utils_js.h"

// Global instance of MakerAPIModule
MakerAPIModule makerAPI;

MakerAPIModule::MakerAPIModule() {}

MakerAPIModule::~MakerAPIModule() {}

void MakerAPIModule::begin() {
  // Nothing needed here - we're just serving UI assets
}

void MakerAPIModule::handle() {
  // Nothing to do in regular processing
}

OpenAPIDocumentation MakerAPIModule::getOpenAPIConfigDocs() {
  OpenAPIDocumentation doc =
      OpenAPIFactory::create("Get OpenAPI configuration",
                             "Retrieves a system information about the "
                             "availability of OpenAPI information.",
                             "getOpenAPIConfig", {"Maker API"});

  doc.responseExample = R"({
      "success": true,
      "OpenApiConfig": {
         fullSpec: true
         makerSpec: true
      }
    })";

  doc.responseSchema =
      OpenAPIFactory::createSuccessResponse("System OpenAPI configuration");

  return doc;
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
  JsonResponseBuilder::createResponse<512>(res, [&](JsonObject &root) {
    root["success"] = true;

    JsonObject config = root.createNestedObject("OpenApiConfig");
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
                            {AuthType::SESSION}));

  // Static assets
  routes.push_back(
      WebRoute("/assets/maker-api-style.css", WebModule::WM_GET,
               [this](WebRequest &req, WebResponse &res) {
                 res.setProgmemContent(MAKER_API_STYLES_CSS, "text/css");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::LOCAL_ONLY}));

  routes.push_back(
      WebRoute("/assets/maker-api-utils.js", WebModule::WM_GET,
               [this](WebRequest &req, WebResponse &res) {
                 res.setProgmemContent(MAKER_API_UTILS_JS,
                                       "application/javascript; charset=utf-8");
                 res.setHeader("Cache-Control", "public, max-age=3600");
               },
               {AuthType::LOCAL_ONLY}));

  routes.push_back(ApiRoute(
      "openapi/config", WebModule::WM_POST,
      [this](WebRequest &req, WebResponse &res) {
        getOpenAPIConfigHandler(req, res);
      },
      {AuthType::PAGE_TOKEN}, API_DOC_BLOCK(getOpenAPIConfigDocs())));

  return routes;
}

std::vector<RouteVariant> MakerAPIModule::getHttpsRoutes() {
  return getHttpRoutes();
}