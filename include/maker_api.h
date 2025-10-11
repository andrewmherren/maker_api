#ifndef MAKER_API_H
#define MAKER_API_H

#include <Arduino.h>
#include <interface/openapi_factory.h>
#include <interface/web_module_interface.h>
#include <interface/web_platform_interface.h>

class MakerAPIModule : public IWebModule {
public:
  // Default constructor - uses global provider instance
  MakerAPIModule();

  // Optional constructor for dependency injection (tests)
  MakerAPIModule(IWebPlatformProvider *provider);

  ~MakerAPIModule();

  // Module lifecycle
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

  OpenAPIDocumentation getOpenAPIConfigDocs();

private:
  // Platform provider (injected or global)
  IWebPlatformProvider *platformProvider;

  // Helper to access the platform
  IWebPlatform &getPlatform() const { return platformProvider->getPlatform(); }

  // Internal handler
  void getOpenAPIConfigHandler(WebRequest &req, WebResponse &res);
};

// Global instance for production builds
extern MakerAPIModule makerAPI;

#endif // MAKER_API_H
