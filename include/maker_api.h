#ifndef MAKER_API_H
#define MAKER_API_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <ArduinoFake.h>
#endif

#include <web_platform_interface.h>

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

  OpenAPIDocumentation getOpenAPIConfigDocs() const;

private:
  // Platform provider (injected or global)
  IWebPlatformProvider *platformProvider;

  // Helper to access the platform
  IWebPlatform &getPlatform() const { return platformProvider->getPlatform(); }

  // Internal handler
  void getOpenAPIConfigHandler(WebRequest &req, WebResponse &res) const;
};

// Global instance for production builds
// NOSONAR - This module instance must be mutable as it maintains state and
// implements lifecycle methods
extern MakerAPIModule makerAPI; // NOSONAR

#endif // MAKER_API_H
