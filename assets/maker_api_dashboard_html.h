#ifndef MAKER_API_DASHBOARD_HTML_H
#define MAKER_API_DASHBOARD_HTML_H

#include <Arduino.h>

// Dashboard HTML template for the Maker API interface
const char MAKER_API_DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Maker API Dashboard - {{DEVICE_NAME}}</title>
    <link rel="icon" href="/assets/favicon.svg" type="image/svg+xml">
    <link rel="icon" href="/assets/favicon.ico" sizes="any">
    <link rel="stylesheet" href="/assets/style.css">
    <link rel="stylesheet" href="{{MODULE_PREFIX}}/assets/maker-api-style.css">
    <script src="/assets/web-platform-utils.js"></script>
</head>
<body>
    <div class="container">
        {{NAV_MENU}}
        
        <div class="page-header">
            <h1>🚀 Maker API Dashboard</h1>
            <p>Interactive API Explorer & Documentation</p>
        </div>
        
        <div class="status-grid">
            <div class="status-card">
                <h3>📊 API Overview</h3>
                <div id="api-stats">
                    <div class="api-stat">
                        <span class="stat-number" id="total-routes">-</span>
                        <span class="stat-label">Total Routes</span>
                    </div>
                    <div class="api-stat">
                        <span class="stat-number" id="total-modules">-</span>
                        <span class="stat-label">Modules</span>
                    </div>
                    <div class="api-stat">
                        <span class="stat-number" id="public-routes">-</span>
                        <span class="stat-label">Public</span>
                    </div>
                    <div class="api-stat">
                        <span class="stat-number" id="protected-routes">-</span>
                        <span class="stat-label">Protected</span>
                    </div>
                </div>
            </div>
            
            <div class="status-card">
                <h3>🔧 Quick Actions</h3>
                <div class="action-buttons">
                    <button id="refresh-routes" class="btn btn-primary">
                        🔄 Refresh Routes
                    </button>
                    <button id="download-openapi" class="btn btn-secondary">
                        📥 Download OpenAPI Spec
                    </button>
                </div>
            </div>
        </div>
        
        <div class="status-card" id="token-section">
            <h3>🔑 API Token</h3>
            <p>Select or enter an API token to test protected endpoints.</p>
            <div class="token-controls">
                <div class="form-group">
                    <select id="token-selector" class="form-control token-selector">
                        <option value="manual">Enter token manually</option>
                        <!-- Other tokens will be loaded dynamically -->
                    </select>
                    <input type="text" id="api-token-input" placeholder="Enter API token..." class="form-control">
                </div>
            </div>
            <div class="token-help">
                <p><strong>💡 How to get an API token:</strong></p>
                <ol>
                    <li>Go to <a href="/account">Account Settings</a></li>
                    <li>Create a new API token</li>
                    <li>Copy the token and paste it here</li>
                </ol>
            </div>
        </div>
        
        <div class="status-card">
            <h2>📋 API Routes</h2>
            <div class="routes-controls">
                <div class="search-container">
                    <input type="text" id="route-search" placeholder="🔍 Search routes..." class="form-control">
                </div>
                <div class="filter-container">
                    <select id="tag-filter" class="form-control">
                        <option value="">All Tags</option>
                        <!-- Tags will be populated dynamically -->
                    </select>
                    <select id="method-filter" class="form-control">
                        <option value="">All Methods</option>
                        <option value="get">GET</option>
                        <option value="post">POST</option>
                        <option value="put">PUT</option>
                        <option value="patch">PATCH</option>
                        <option value="delete">DELETE</option>
                    </select>
                </div>
            </div>
            
            <div id="loading-indicator" class="loading-state">
                <div class="loading-spinner"></div>
                <p>Loading API routes...</p>
            </div>
            
            <div id="routes-container" class="api-endpoints" style="display: none;">
                <!-- Routes will be populated here by JavaScript -->
            </div>
            
            <div id="no-routes-message" class="empty-state" style="display: none;">
                <p>📭 No routes found matching your filters.</p>
                <button id="clear-filters" class="btn btn-primary">Clear Filters</button>
            </div>
            
            <div id="error-message" class="error-state" style="display: none;">
                <p>❌ Failed to load API routes.</p>
                <button id="retry-load" class="btn btn-primary">Retry</button>
            </div>
        </div>
        
        <div class="footer">
            <p>Maker API Dashboard - OpenAPI 3.0 Compatible</p>
            <p>Device: <strong>{{DEVICE_NAME}}</strong> | User: <strong>{{username}}</strong></p>
        </div>
    </div>
    
    <script src="{{MODULE_PREFIX}}/assets/maker-api-utils.js"></script>
    
</body>
</html>
)rawliteral";

#endif // MAKER_API_DASHBOARD_HTML_H