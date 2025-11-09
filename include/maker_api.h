#ifndef MAKER_API_H
#define MAKER_API_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <interface/auth_types.h>
#include <interface/request_response_types.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <utility>
#include <web_platform_interface.h>
#include "version_autogen.h"

/**
 * @brief Maker API Module - Interactive API documentation and testing interface
 * 
 * Provides a web-based interface for exploring and testing API endpoints
 * with OpenAPI integration for maker-friendly documentation.
 */
class MakerAPIModule : public IWebModule {
public:
  // Default constructor - uses global provider instance
  MakerAPIModule();

  // Optional constructor for dependency injection (tests)
  explicit MakerAPIModule(IWebPlatformProvider *provider);

  ~MakerAPIModule() override;

  // Module lifecycle
  using IWebModule::begin; // Bring base class overloads into scope
  void begin() override;
  void handle() override;

  // IWebModule interface
  std::vector<RouteVariant> getHttpRoutes() override;
  std::vector<RouteVariant> getHttpsRoutes() override;
  String getModuleName() const override { return "Maker API"; }
  // Version must be injected at build time from library.json as WEB_MODULE_VERSION_STR
  // via PlatformIO extra_script. Intentionally fail build if missing.
#ifndef WEB_MODULE_VERSION_STR
#error "WEB_MODULE_VERSION_STR not defined (version_autogen.h missing)."
#endif
  String getModuleVersion() const override { return WEB_MODULE_VERSION_STR; }
  String getModuleDescription() const override {
    return "API documentation and testing interface for makers";
  }

  // RequestT/ResponseT are provided by <interface/request_response_types.h>

  // Route handler methods (unified signatures for ESP32 and native platforms)
  void getOpenAPIConfigHandler(RequestT &req, ResponseT &res) const;
  
  // OpenAPI documentation
  OpenAPIDocumentation getOpenAPIConfigDocs() const;

  // Helper for testing - get OpenAPI config status without handler overhead
  struct OpenAPIConfigStatus {
    bool fullSpec;
    bool makerSpec;
  };
  OpenAPIConfigStatus getOpenAPIStatus() const;

private:
  // Platform provider (injected or global)
  IWebPlatformProvider *platformProvider;

  // Helper to access the platform
  IWebPlatform &getPlatform() const { return platformProvider->getPlatform(); }

  // Helper to reduce platform lookup duplication when creating JSON responses
  template <typename Fn>
  inline void respondJson(ResponseT &res, Fn &&fn) const {
    IWebPlatformProvider::getPlatformInstance().createJsonResponse(
        res, std::forward<Fn>(fn));
  }
};

// Global instance for production builds
// NOSONAR - This module instance must be mutable as it maintains state and
// implements lifecycle methods
#if defined(ARDUINO) || defined(ESP_PLATFORM)
extern MakerAPIModule makerAPI; // NOSONAR
#endif

#endif // MAKER_API_H
