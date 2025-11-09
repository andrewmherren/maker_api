#ifndef MAKER_API_H
#define MAKER_API_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <interface/auth_types.h>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <utility>
#include <web_platform_interface.h>

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
  String getModuleVersion() const override { return "0.1.0"; }
  String getModuleDescription() const override {
    return "API documentation and testing interface for makers";
  }

  // Platform-specific request/response typedefs for unified handlers
#if defined(ARDUINO) || defined(ESP_PLATFORM)
  using RequestT = WebRequest;
  using ResponseT = WebResponse;
#else
  using RequestT = WebRequestCore;
  using ResponseT = WebResponseCore;
#endif

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
