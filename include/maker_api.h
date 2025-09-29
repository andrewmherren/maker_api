#ifndef MAKER_API_H
#define MAKER_API_H

#include <Arduino.h>
#include <web_platform.h>

class MakerAPIModule : public IWebModule {
public:
  MakerAPIModule();
  ~MakerAPIModule();

  void begin() override;
  void handle() override;

  // IWebModule interface implementation
  std::vector<RouteVariant> getHttpRoutes() override;
  std::vector<RouteVariant> getHttpsRoutes() override;
  String getModuleName() const override { return "Maker API"; }
  String getModuleVersion() const override { return "0.1.0"; }
  String getModuleDescription() const override {
    return "API documentation and testing interface for makers";
  }
  OpenAPIDocumentation getOpenAPIConfigDocs();

private:
  void getOpenAPIConfigHandler(WebRequest &req, WebResponse &res);
};

// Global instance
extern MakerAPIModule makerAPI;

#endif // MAKER_API_H