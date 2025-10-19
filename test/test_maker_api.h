#ifndef TEST_MAKER_API_H
#define TEST_MAKER_API_H

// Forward declarations for core type tests
void test_maker_api_module_properties();
void test_maker_api_module_lifecycle();
void test_maker_api_http_routes();
void test_maker_api_https_routes();
void test_maker_api_openapi_docs();
void test_maker_api_config_api_handler();
void test_maker_api_compilation_flags();
void test_constructor_with_provider();
void test_openapi_config_handler_with_flags();
void test_static_asset_routes();
void test_module_platform_integration();

void register_maker_api_tests();

#endif // TEST_MAKER_API_H