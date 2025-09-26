# Maker API Implementation Plan

This document outlines the implementation phases for the Maker API module. Each phase represents a complete, testable increment.

## Phase 1: OpenAPI Generation in WebPlatform (complete)

**Goal**: Implement OpenAPI 3.0 specification generation directly in WebPlatform

### Tasks
1. **Extend IPlatformService interface** (`lib/web_platform/include/interface/platform_service.h`)
   - Add `getOpenAPISpec()` method returning JSON string
   - Add `getOpenAPISpec(AuthType filterType)` for filtered specs
   - Add `generateDisableRouteCode()` method for route management

2. **Enhance WebRoute structure** (`lib/web_platform/include/interface/web_module_interface.h`)
   - Add OpenAPI-compatible fields: `summary`, `operationId`, `parameters`, `responseInfo`, `tags`
   - Define helper structures: `ParameterInfo`, `ResponseInfo` for documentation
   - Add constructor overloads for enhanced documentation
   - Maintain backward compatibility with existing constructors

3. **Implement OpenAPI generation** (`lib/web_platform/src/web_platform.cpp`)
   - Create OpenAPI 3.0 JSON specification generator
   - Convert route registry to OpenAPI paths, parameters, responses
   - Apply filtering (auth type, disabled routes)
   - Include device-specific server information
   - Generate operation IDs automatically if missing

4. **Add route documentation validation** (`lib/web_platform/src/web_platform.cpp`)
   - Warning system for missing documentation on token routes
   - Validation of OpenAPI structure completeness
   - Integration into module registration process

**Files to create/modify**:
- `lib/web_platform/include/interface/platform_service.h` (modify)
- `lib/web_platform/include/interface/web_module_interface.h` (modify)  
- `lib/web_platform/src/web_platform.cpp` (modify)

**Testing**: Verify OpenAPI JSON is valid, can be imported into Postman, contains expected routes

## Phase 2: Basic Module Structure with OpenAPI Integration (complete)

**Goal**: Create minimal working Maker API module that consumes OpenAPI specifications

### Tasks
1. **Create module directory structure**
   - `lib/maker_api/include/maker_api.h`
   - `lib/maker_api/src/maker_api.cpp`
   - `lib/maker_api/library.json`

2. **Implement MakerAPIModule class**
   - Inherit from `IWebModule`
   - Implement required interface methods
   - Add platform service integration point
   - Create basic route structure: `/`, `/api/routes`, `/openapi.json`

3. **OpenAPI consumption functionality**
   - Receive platform service reference
   - Parse OpenAPI JSON from platform service
   - Extract route information for display
   - Cache parsed specification for performance

4. **OpenAPI serving endpoints**
   - `/openapi.json`: Pass-through filtered OpenAPI spec from platform
   - `/api/routes`: Extract and return simplified route list for UI consumption
   - Basic route information: path, method, auth, module, summary

**Files to create**:
- `lib/maker_api/include/maker_api.h`
- `lib/maker_api/src/maker_api.cpp`
- `lib/maker_api/library.json`

**Testing**: Verify module loads, OpenAPI spec is served correctly, route parsing works

## Phase 3: Basic HTML Interface (complete)

**Goal**: Create functional web interface for viewing API routes

### Tasks
1. **Create HTML template system**
   - Main dashboard HTML with WebPlatform template integration
   - Route card layout using glass morphism CSS
   - Integration with existing navigation system

2. **Basic route display**
   - Route cards showing: method, path, description, module
   - Authentication type indicators
   - Module-based grouping
   - Responsive design using WebPlatform CSS framework

3. **Static asset integration**
   - CSS for API documentation styling (`assets/maker_api_styles_css.h`)
   - Basic JavaScript for interface interaction
   - Integration with WebPlatform asset serving pattern

4. **Template processing integration**
   - Use WebPlatform's bookmark replacement system
   - Navigation menu integration
   - Authentication-aware display

**Files to create**:
- `lib/maker_api/assets/maker_api_styles_css.h`
- `lib/maker_api/assets/maker_api_dashboard_js.h`
- HTML template generation in `maker_api.cpp`

**Testing**: Verify web interface loads, displays routes correctly, styling works

## Phase 4: Token Input and API Testing (complete)

**Goal**: Add interactive API testing functionality using OpenAPI specification

### Tasks
1. **Token management interface**
   - Token input field with validation
   - Session-based token storage (not localStorage)
   - Token testing/validation endpoint

2. **OpenAPI-driven API testing proxy**
   - `/api/test` endpoint for proxied API calls
   - Use OpenAPI spec to validate request parameters
   - Dynamic parameter processing based on OpenAPI schema
   - Response formatting and error handling
   - Security validation (token required, same-origin)

3. **Interactive testing UI with OpenAPI integration**
   - "Try It" buttons on route cards
   - Dynamic parameter input forms generated from OpenAPI schema
   - Parameter validation using OpenAPI constraints
   - Response display area with JSON formatting
   - Loading states and error handling

4. **JavaScript enhancement**
   - OpenAPI spec parsing for dynamic form generation
   - AJAX calls for API testing
   - Response formatting and syntax highlighting
   - Form validation based on OpenAPI parameter definitions
   - Integration with WebPlatform's CSRF system

**Files to modify/create**:
- Update `maker_api.cpp` with testing endpoints and OpenAPI parsing
- Enhance JavaScript assets with OpenAPI-driven form generation
- Add response formatting utilities

**Testing**: Verify token validation, dynamic forms work, API calls succeed, responses display correctly

## Phase 5: Route Management Features (complete)

**Goal**: Add route disable functionality and code generation using OpenAPI metadata

### Tasks
1. **Route disable code generation using OpenAPI**
   - Use OpenAPI operationId and path information for accurate code generation
   - Create `/api/disable-code` endpoint in module
   - Extract route details from OpenAPI spec for code generation
   - Code snippet formatting and escaping

2. **OpenAPI-driven route management UI**
   - "Manage Route" button/modal for each route
   - Display route information from OpenAPI specification
   - "Disable Route" action with code display
   - Copy-to-clipboard functionality
   - Clear instructions for code placement

3. **Enhanced route information display from OpenAPI**
   - Expandable route details showing full OpenAPI documentation
   - Parameter documentation from OpenAPI schema
   - Response schema display from OpenAPI responses
   - Tags and operation metadata display
   - Override/disabled status indicators

4. **Modal system integration**
   - Reusable modal component
   - Integration with WebPlatform CSS framework
   - Keyboard navigation and accessibility

**Files to modify**:
- Add modal templates and JavaScript with OpenAPI integration
- Enhance route display with OpenAPI metadata
- Add clipboard integration utilities

**Testing**: Verify code generation accuracy using OpenAPI data, modal functionality, clipboard operations

## Phase 6: Advanced Documentation Features (complete)

**Goal**: Enhance OpenAPI documentation with examples and validation

### Tasks
1. **Enhanced OpenAPI documentation in WebPlatform**
   - Add example request/response bodies to route documentation
   - Parameter examples and validation constraints
   - Better tag organization and descriptions
   - Schema definitions for complex objects

2. **Documentation quality validation**
   - OpenAPI spec validation against 3.0 schema
   - Enhanced warnings for incomplete documentation
   - Suggestions for improving API documentation

3. **External tool integration guidance**
   - Documentation for Postman import process
   - Code generation tool integration examples
   - Best practices for OpenAPI consumption

4. **Module documentation examples**
   - Update example modules with rich OpenAPI documentation
   - Templates for common API patterns
   - Integration examples in README

**Files to modify**:
- Enhance OpenAPI generation in WebPlatform
- Add documentation validation utilities
- Update examples and documentation

**Testing**: Validate enhanced spec with external tools, verify documentation quality

## Phase 7: Advanced UI Features

**Goal**: Polish interface with advanced features and optimizations

### Tasks
1. **Enhanced testing features**
   - cURL command generation
2. **Performance optimizations**
   - Lazy loading of route details
   - Efficient DOM updates
   - Optimized asset delivery
   - Caching strategies
3. Ensure code is production ready with no temporary hacks or dev only comments or logging

**Files to modify**:
- Enhance JavaScript with advanced features
- Add search and filter utilities
- Optimize CSS for mobile devices
- Add accessibility attributes

**Testing**: Cross-browser testing, mobile device testing, accessibility validation

## Implementation Notes

### File Organization
```
lib/maker_api/
├── include/
│   └── maker_api.h
├── src/
│   └── maker_api.cpp
├── assets/
│   ├── maker_api_styles_css.h
│   ├── maker_api_dashboard_js.h
│   └── maker_api_utils_js.h
├── examples/
│   └── basic_usage/
├── library.json
├── README.md
├── PLAN.md
└── OVERVIEW.md
```

### Key Design Principles
- **OpenAPI-Native**: OpenAPI 3.0 is the canonical format throughout the system
- **Backward Compatibility**: All changes to WebPlatform must not break existing modules
- **Progressive Enhancement**: Rich documentation is optional but encouraged with warnings
- **Security First**: All testing goes through proper authentication
- **Standards Compliance**: Full OpenAPI 3.0 compatibility from day one

### Testing Strategy
- Each phase should be fully functional and testable
- Integration testing with sample modules
- External tool validation (Postman, curl)

### Dependencies
- WebPlatform (parent framework with OpenAPI generation)
- ArduinoJson (for OpenAPI JSON parsing and processing)
- No external HTTP client libraries (use platform service for testing)

Each phase represents approximately 1-2 days of focused development work and should result in demonstrable functionality.