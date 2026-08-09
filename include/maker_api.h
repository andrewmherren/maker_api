#ifndef MAKER_API_H
#define MAKER_API_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <ArduinoFake.h>
#endif

#include <interface/request_response_types.h>
#include <web_platform_interface.h>
#include "version_autogen.h"

// Version must be injected at build time from library.json as
// WEB_MODULE_VERSION_STR via PlatformIO extra_script. Intentionally fail
// build if missing.
#ifndef WEB_MODULE_VERSION_STR
#error "WEB_MODULE_VERSION_STR not defined (version_autogen.h missing)."
#endif

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
  String getModuleVersion() const override { return WEB_MODULE_VERSION_STR; }
  String getModuleDescription() const override {
    return "API documentation and testing interface for makers";
  }

  OpenAPIDocumentation getOpenAPIConfigDocs() const;

private:
  // Platform provider (injected or global)
  IWebPlatformProvider *platformProvider;

  // Helper to access the platform
  IWebPlatform &getPlatform() const { return platformProvider->getPlatform(); }

  // Internal handler
  void getOpenAPIConfigHandler(RequestT &req, ResponseT &res) const;
};

// Global instance for production builds
// NOSONAR - This module instance must be mutable as it maintains state and
// implements lifecycle methods
extern MakerAPIModule makerAPI; // NOSONAR

#endif // MAKER_API_H
