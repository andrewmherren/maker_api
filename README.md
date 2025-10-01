# [Web Platform](https://www.github.com/andrewmherren/web_platform) - Interactive API Explorer for WebPlatform

A comprehensive web module that provides an interactive API explorer interface for WebPlatform-enabled ESP32 devices. Think Swagger/Postman for your embedded projects, specifically designed for "maker-friendly" public APIs.

## ✨ Features

- 🔧 **Interactive Route Management**: Modal-based interface for route operations
- 📝 **OpenAPI-Driven Code Generation**: Accurate disable/override code with metadata
- 📋 **Copy-to-Clipboard**: One-click code copying with browser fallback
- 📊 **Enhanced Route Details**: Comprehensive information from OpenAPI specifications
- 🔍 **Maker API Specification**: Dedicated, filtered OpenAPI spec for public APIs
- 🔄 **Tab-Based Management**: Organized disable, override, and info sections
- 🚀 **Interactive API Testing**: Test token-protected endpoints directly in browser
- 📚 **Public API Documentation**: Automatic API specification generation for public endpoints
- 🔍 **Visual Route Explorer**: Browse maker-friendly APIs with search and filtering
- 🎯 **Token Management**: Secure API token validation and storage
- 📱 **Responsive Design**: Works on desktop and mobile devices
- 🔒 **Security-First**: Session-based authentication with proper CSRF protection

## Quick Start

### Build Configuration

Enable the Maker API OpenAPI specification generation in your `platformio.ini`:

```ini
build_flags = 
    ; ... other flags ...
    -DWEB_PLATFORM_MAKERAPI=1
```

### Installation

Add the Maker API module to your WebPlatform project:

```cpp
#include <web_platform.h>
#include <maker_api.h>

MakerAPIModule makerAPI;

void setup() {
  Serial.begin(115200);
  
  // Set up navigation with API explorer link
  std::vector<NavigationItem> navItems = {
    NavigationItem("Home", "/"),
    NavigationItem("API Explorer", "/api-explorer/"),
    Authenticated(NavigationItem("Account", "/account")),
    Authenticated(NavigationItem("Logout", "/logout"))
  };
  webPlatform.setNavigationMenu(navItems);
  
  // Initialize WebPlatform
  webPlatform.begin("MyDevice");
  
  if (webPlatform.isConnected()) {
    // Register the API explorer module with default tag configuration (only "maker" tag)
    webPlatform.registerModule("/api-explorer", &makerAPI);
    
    // OR - Register with custom tags configuration
    // This will include routes tagged with "maker", "public", or "external" in the Maker API spec
    DynamicJsonDocument tagsConfig(256);
    JsonArray tagsArray = tagsConfig.createNestedArray("tags");
    tagsArray.add("maker");
    tagsArray.add("public");
    tagsArray.add("external");
    webPlatform.registerModule("/api-explorer", &makerAPI, tagsConfig);
  }
}

void loop() {
  webPlatform.handle();
  delay(10); // Allow ESP32 system tasks to run
}
```

### Access the Interface

1. Connect to your device's web interface
2. Navigate to `/api-explorer/` or use the navigation menu
3. Log in with your WebPlatform credentials
4. Explore and test your device's maker-friendly API endpoints

## Maker API Concept

The Maker API module provides a focused OpenAPI specification containing only routes that are intended for public consumption. This is ideal for maker projects that need:

- **Public API Documentation**: A clean, focused API spec for end users
- **Hackable Interfaces**: Well-documented endpoints for DIY integration
- **Clear Separation**: Public vs. internal API endpoints
- **Simplified Documentation**: Only what makers need, not the entire system

### How It Works

1. Routes tagged with specified "maker" tags are included in a dedicated OpenAPI spec
2. A streamlined `/maker/openapi.json` endpoint serves only public API details
3. The API explorer interface shows only these maker-friendly endpoints
4. Users can explore, test, and integrate with your device's public interfaces

## Tagging Routes for Maker API

To mark a route for inclusion in the Maker API, add the "maker" tag (or custom configured tags):

```cpp
// Basic route with maker tag
ApiRoute("/device/status", WebModule::WM_GET, statusHandler, {AuthType::TOKEN},
         API_DOC("Get device status", "Returns current device operational status", 
                 "getStatus", {"maker"})); // The "maker" tag includes it in Maker API

// Multiple tags example
ApiRoute("/device/control", WebModule::WM_POST, controlHandler, {AuthType::TOKEN},
         API_DOC("Control device", "Send commands to the device", 
                 "controlDevice", {"maker", "control"}));
```

Routes in the `/maker/` path are automatically included regardless of tags:

```cpp
// Automatically included in Maker API due to path prefix
ApiRoute("/maker/data", WebModule::WM_GET, dataHandler, {AuthType::TOKEN});
```

## Configuration Options

### Customizing Included Tags

By default, only routes with the "maker" tag are included. To customize which tags are included:

```cpp
// Include routes with "maker", "public", or "external" tags
DynamicJsonDocument tagsConfig(256);
JsonArray tagsArray = tagsConfig.createNestedArray("tags");
tagsArray.add("maker");
tagsArray.add("public");
tagsArray.add("external");
webPlatform.registerModule("/api-explorer", &makerAPI, tagsConfig);
```

### Build Flags

The Maker API functionality requires the `WEB_PLATFORM_MAKERAPI` build flag:

```ini
# platformio.ini
build_flags = 
    ; ... other flags ...
    -DWEB_PLATFORM_MAKERAPI=1
```

This flag is independent from the standard OpenAPI flag (`WEB_PLATFORM_OPENAPI`), allowing various configurations:

- **Both Enabled**: Full internal API docs + filtered Maker API docs
- **Only Maker Enabled**: Only public/maker API docs available
- **Only OpenAPI Enabled**: Only full internal API docs available
- **Both Disabled**: No API documentation generated (minimal memory usage)

## OpenAPI 3.0 Integration

The module generates a dedicated Maker API OpenAPI 3.0 specification available at `/maker/openapi.json`. This focused spec can be used with:

- **Postman**: Import just the public API endpoints
- **External Documentation**: Generate maker-friendly documentation sites
- **Code Generators**: Auto-generate client libraries for just public APIs
- **API Testing Tools**: Automated testing of public endpoints

### Example OpenAPI Usage

```bash
# Download the Maker API spec
curl https://mydevice.local/maker/openapi.json > device-maker-api.json

# Import into Postman or other tools
# Generate client libraries for just the public APIs
# Create automated tests for maker endpoints
```

## Memory Efficiency

The Maker API implementation is designed for optimal memory usage:

- Separate JSON document with smaller size for Maker API spec generation
- Memory-efficient streaming of stored spec without parsing at request time
- Only generated once during initialization
- Zero heap fragmentation through proper storage and streaming
- Optional compilation with build flags

## Enhanced Route Documentation

The Maker API module encourages rich route documentation for maker-friendly APIs:

```cpp
// Fully documented maker route example
ApiRoute("/device/reading", WebModule::WM_GET, 
         sensorHandler, {AuthType::TOKEN},
         API_DOC_BLOCK(
           []() {
             auto doc = OpenAPIDocumentation(
               "Get sensor reading", 
               "Returns the current sensor value with timestamp and unit information",
               "getSensorReading",
               {"maker", "sensor"}  // Tags this as a maker API
             );
             
             doc.parameters = R"([
               {
                 "name": "sensor_id",
                 "in": "query",
                 "required": false,
                 "description": "Specific sensor ID to query (defaults to primary sensor)",
                 "schema": { "type": "string" }
               }
             ])";
             
             doc.responseExample = R"({
               "value": 23.5,
               "unit": "celsius",
               "timestamp": "2023-06-15T14:22:31Z",
               "sensor_id": "temp1"
             })";
             
             return doc;
           }()
         )
```

## Security Considerations

- **Session Authentication Required**: The API explorer interface requires login
- **Token Validation**: All API testing goes through proper token validation
- **No Token Storage**: Tokens are not stored in browser localStorage for security
- **Route Filtering**: Only displays routes that are specifically tagged for makers
- **Disabled Route Hiding**: Disabled routes are completely hidden from the interface

## Troubleshooting

### Common Issues

**No Maker API spec generated**: Ensure `WEB_PLATFORM_MAKERAPI=1` build flag is set
**Routes not appearing in Maker API**: Check that routes have the correct tag ("maker" by default)
**Custom tags not working**: Verify tag array syntax in module registration
**Missing routes with /maker/ path**: Ensure they have the proper auth settings
**Testing fails**: Verify API token is valid and has required permissions

### Debug Output

Enable debug output to see Maker API route discovery and tag filtering:

```cpp
// In setup(), before webPlatform.begin()
Serial.setDebugOutput(true);
```

The debug output will show which tags are being used for filtering and which routes are included in the Maker API spec.

## Development and Best Practices

### Tag Naming Conventions

- **"maker"**: Primary tag for maker-friendly endpoints
- **"public"**: Alternative for publicly accessible endpoints
- **"external"**: For endpoints intended for external systems
- **"hackable"**: For endpoints specifically designed for maker experimentation
- **"integration"**: For endpoints intended for integration with other systems

### Route Design for Maker APIs

1. **Stability**: Maker API endpoints should maintain backward compatibility
2. **Clear Documentation**: Always provide detailed descriptions and examples
3. **Error Handling**: Return descriptive error messages with proper status codes
4. **Simplicity**: Keep parameter requirements minimal and intuitive
5. **Consistency**: Use consistent naming and response formats

## License

This module is part of the WebPlatform ecosystem and follows the same licensing terms.