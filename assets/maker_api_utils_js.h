#ifndef MAKER_API_UTILS_JS_H
#define MAKER_API_UTILS_JS_H

#include <Arduino.h>

// Simplified Maker API JavaScript - consolidated functionality
const char MAKER_API_UTILS_JS[] PROGMEM = R"rawliteral(
/**
 * MakerAPI - Simplified Implementation
 * All functionality consolidated into a single, clean interface
 */

const MakerAPI = {
  // State management
  state: {
    routes: [],
    loading: false,
    error: null,
    token: null,
    availableTokens: [],
    openApiSpec: null,
    allTags: null,  // Cache for all available tags
    openApiConfig: null,  // OpenAPI configuration from server
    selectedSpec: 'maker',  // Default to maker spec
    availableSpecs: []  // List of available specs
  },
  
  // Initialize the dashboard
  async init() {
    this.setupUI();
    
    // Initialize token UI first so it's usable even if other steps fail
    this.updateTokenSelector();
    
    // Start token loading process - but don't await it directly
    // This allows the UI to be responsive even if token loading fails
    this.loadAvailableTokens().catch(error => {
      console.warn('Token loading failed but continuing with app initialization:', error);
    });
    
    // Check OpenAPI configuration first
    try {
      await this.loadOpenApiConfiguration();
      
      // If no specs are available, show message and stop
      if (this.state.availableSpecs.length === 0) {
        this.showNoSpecsMessage();
        this.setupEventListeners();
        this.setupGlobalEventHandlers();
        return;
      }
      
      // Setup spec selector UI if multiple specs available
      if (this.state.availableSpecs.length > 1) {
        this.setupSpecSelector();
      }
      
      // Load routes with the selected/available spec
      await this.loadRoutes();
      
    } catch (error) {
      console.error('Failed to load OpenAPI configuration:', error);
      this.showError('Failed to load OpenAPI configuration. Please refresh to try again.');
    }
    
    this.setupEventListeners();
    this.setupGlobalEventHandlers();
  },
  
  // Load OpenAPI configuration from server
  async loadOpenApiConfiguration() {
    try {
      const response = await AuthUtils.fetch('/maker_api/api/config', {
        method: 'POST',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
          'X-Requested-With': 'XMLHttpRequest'
        },
        credentials: 'include',
        body: JSON.stringify({})
      });
      
      if (!response.ok) {
        throw new Error(`Failed to fetch OpenAPI config: HTTP ${response.status}`);
      }
      
      const data = await response.json();
      this.state.openApiConfig = data.OpenApiConfig || {};
      
      // Determine available specs
      this.state.availableSpecs = [];
      if (this.state.openApiConfig.fullSpec) {
        this.state.availableSpecs.push({id: 'full', name: 'Full API Specification', url: '/openapi.json'});
      }
      if (this.state.openApiConfig.makerSpec) {
        this.state.availableSpecs.push({id: 'maker', name: 'Maker API Specification', url: '/maker/openapi.json'});
      }
      
      // Set default selected spec
      if(!this.state.selectedSpec || !this.state.availableSpecs.includes(this.state.selectedSpec)) {
        if (this.state.availableSpecs.length === 1) {
          this.state.selectedSpec = this.state.availableSpecs[0].id;
        } else if (this.state.availableSpecs.length > 1) {
          // Default to maker spec if available, otherwise first available
          const makerSpec = this.state.availableSpecs.find(spec => spec.id === 'maker');
          this.state.selectedSpec = makerSpec ? 'maker' : this.state.availableSpecs[0].id;
        }
      }
      
    } catch (error) {
      console.error('Error fetching OpenAPI configuration:', error);
      throw error;
    }
  },

  // Load API routes from OpenAPI specification
  async loadRoutes() {
    this.showLoading(true);
    
    try {
      // Fetch OpenAPI spec from the selected endpoint
      this.state.openApiSpec = await this.fetchOpenAPISpec();
      
      // Parse routes from OpenAPI paths
      this.state.routes = this.parseRoutesFromOpenAPI(this.state.openApiSpec);
      this.state.error = null;
      
      this.updateStats();
      this.renderRoutes();
      this.showLoading(false);
      
      // Update server info with new spec data
      this.updateServerInfo();
            
    } catch (error) {
      console.error('Failed to load routes:', error);
      this.state.error = error.message;
      this.showError(error.message);
    }
  },
  


  // Parse routes from OpenAPI specification
  parseRoutesFromOpenAPI(spec) {
    const routes = [];
    
    if (!spec.paths) return routes;
    
    for (const [path, pathItem] of Object.entries(spec.paths)) {
      for (const [method, operation] of Object.entries(pathItem)) {
        if (!operation || typeof operation !== 'object') continue;
        
        // Extract authentication types - support multiple auth types
        let authTypes = ['none'];
        if (operation.security && operation.security.length > 0) {
          authTypes = [];
          operation.security.forEach(security => {
            if (security.bearerAuth) {
              authTypes.push('token');
            }
            if (security.cookieAuth) {
              authTypes.push('session');
            }
            if (security.localAuth) {
              authTypes.push('local_only');
            }
          });
          // If no recognized auth types found, default to 'none'
          if (authTypes.length === 0) {
            authTypes = ['none'];
          }
        }
        
        // Determine display auth type - use 'mixed' if multiple types
        let authType = authTypes.length > 1 ? 'mixed' : authTypes[0];
        
        // Extract module name from tags
        let moduleName = 'Platform';
        if (operation.tags && operation.tags.length > 0) {
          moduleName = operation.tags[0];
        }
        
        routes.push({
          path: path,
          method: method.toUpperCase(),
          summary: operation.summary || '',
          description: operation.description || operation.summary || '',
          authType: authType,
          authTypes: authTypes, // Store all supported auth types
          module: moduleName,
          tags: operation.tags || [moduleName],
          operationId: operation.operationId || '',
          parameters: operation.parameters || []
        });
      }
    }
    
    return routes;
  },
  
  // Render routes in the UI
  renderRoutes() {
    const container = document.getElementById('routes-container');
    const noRoutesMsg = document.getElementById('no-routes-message');
    
    if (!container) return;
    
    if (this.state.routes.length === 0) {
      container.style.display = 'none';
      if (noRoutesMsg) noRoutesMsg.style.display = 'block';
      return;
    }
    
    container.style.display = 'block';
    if (noRoutesMsg) noRoutesMsg.style.display = 'none';
    
    // Group routes by module
    const grouped = this.groupRoutesByModule(this.state.routes);
    
    // Update tag filter dropdown
    this.updateTagFilter();
    
    // Render HTML
    container.innerHTML = Object.entries(grouped)
      .map(([module, routes]) => this.renderModuleSection(module, routes))
      .join('');
  },
  
  // Group routes by module
  groupRoutesByModule(routes) {
    return routes.reduce((acc, route) => {
      const module = this.formatModuleName(route.module || 'Platform');
      if (!acc[module]) acc[module] = [];
      acc[module].push(route);
      return acc;
    }, {});
  },
  
  // Format module name
  formatModuleName(name) {
    return name.replace(/[-_]/g, ' ').replace(/\b\w/g, l => l.toUpperCase());
  },
  
  // Render a module section
  renderModuleSection(module, routes) {
    const icon = this.getModuleIcon(module);
    const sectionId = this.getSectionId(module);
    const isCollapsed = this.getSectionCollapsedState(sectionId);
    const routeCards = routes.map(route => this.renderRouteCard(route)).join('');
    
    return `
      <div class="api-section" data-section="${sectionId}">
        <h3 class="api-section-title" onclick="MakerAPI.toggleSection('${sectionId}')">
          <span class="section-toggle ${isCollapsed ? 'collapsed' : 'expanded'}">▼</span>
          ${icon} ${module}
          <span class="route-count">(${routes.length})</span>
        </h3>
        <div class="api-section-content ${isCollapsed ? 'collapsed' : 'expanded'}">
          ${routeCards}
        </div>
      </div>
    `;
  },
  
  // Render a single route card
  renderRouteCard(route) {
    // Normalize the auth type - use authType if available, otherwise fallback to auth
    const authType = route.authType || route.auth || 'none';
    const routeId = this.generateRouteId(route);
    
    return `
      <div class="api-endpoint" data-method="${route.method.toLowerCase()}" data-auth="${authType}" data-route-id="${routeId}">
        <div class="api-endpoint-header" onclick="MakerAPI.toggleEndpoint('${routeId}')">
          <span class="api-method ${route.method.toLowerCase()}">${route.method.toUpperCase()}</span>
          <span class="api-path" data-full-path="${this.escapeHtml(route.path)}" title="${this.escapeHtml(route.path)}">${this.escapeHtml(route.path)}</span>
          <span class="api-description">${this.escapeHtml(route.summary || route.description || 'No description available')}</span>
          ${this.getAuthTypesDisplay(route)}
          <span class="endpoint-expand-indicator">▼</span>
        </div>
        <div class="api-endpoint-content" onclick="event.stopPropagation()">
          ${this.renderEndpointContent(route)}
        </div>
      </div>
    `;
  },
  
  // Get module icon
  getModuleIcon(module) {
    const icons = {
      'Platform': '🏗️',
      'Usb Pd Control': '⚡',
      'Web Platform': '🌐',
      'Maker Api': '🚀',
      'System': '⚙️'
    };
    return icons[module] || '📦';
  },
  
  // Get auth icon
  getAuthIcon(auth) {
    const icons = {
      'none': '🌍',
      'session': '🔒',
      'token': '🔑',
      'local_only': '🏠',
      'mixed': '🔐'
    };
    return icons[auth || 'none'] || '❓';
  },
  
  // Get auth label
  getAuthLabel(auth) {
    const labels = {
      'none': 'Public',
      'session': 'Session',
      'token': 'Token',
      'local_only': 'Local Only',
      'mixed': 'Mixed'
    };
    return labels[auth || 'none'] || 'Unknown';
  },
  
  // Get formatted auth types for display
  getAuthTypesDisplay(route) {
    const authTypes = route.authTypes || [route.authType || 'none'];
    if (authTypes.length === 1) {
      const authType = authTypes[0];
      return `<span class="api-auth-indicator api-auth-${authType}">
        ${this.getAuthIcon(authType)} ${this.getAuthLabel(authType)}
      </span>`;
    }
    
    // Multiple auth types - show them all
    const authTags = authTypes.map(authType => 
      `<span class="api-auth-tag api-auth-${authType}" title="${this.getAuthLabel(authType)}">
        ${this.getAuthIcon(authType)}
      </span>`
    ).join('');
    
    return `<span class="api-auth-indicator api-auth-mixed">
      ${authTags} Mixed
    </span>`;
  },
  
  // Update statistics
  updateStats() {
    const stats = {
      totalRoutes: this.state.routes.length,
      totalModules: new Set(this.state.routes.map(r => r.module || 'Platform')).size,
      publicRoutes: this.state.routes.filter(r => !r.authType || r.authType === 'none').length,
      protectedRoutes: this.state.routes.filter(r => r.authType && r.authType !== 'none').length
    };
    
    const elements = {
      'total-routes': stats.totalRoutes,
      'total-modules': stats.totalModules,
      'public-routes': stats.publicRoutes,
      'protected-routes': stats.protectedRoutes
    };
    
    Object.entries(elements).forEach(([id, value]) => {
      const el = document.getElementById(id);
      if (el) el.textContent = value;
    });
  },
  
  // Setup event listeners
  setupEventListeners() {
    // Refresh routes
    const refreshBtn = document.getElementById('refresh-routes');
    if (refreshBtn) {
      refreshBtn.addEventListener('click', () => this.refreshRoutes());
    }
      
    const retryBtn = document.getElementById('retry-load');
    if(retryBtn) {
      retryBtn.addEventListener('click', () => this.refreshRoutes());
    }
    
    // Download OpenAPI spec
    const downloadBtn = document.getElementById('download-openapi');
    if (downloadBtn) {
      downloadBtn.addEventListener('click', () => this.downloadOpenAPISpec());
    }
    
    // Token management
    const tokenSelector = document.getElementById('token-selector');
    const tokenInput = document.getElementById('api-token-input');
    
    if (tokenSelector) {
      tokenSelector.addEventListener('change', (e) => this.onTokenSelectionChange(e));
    }
    
    // Filters
    const searchInput = document.getElementById('route-search');
    const tagFilter = document.getElementById('tag-filter');
    const methodFilter = document.getElementById('method-filter');
    const clearFiltersBtn = document.getElementById('clear-filters');
    
    if (searchInput) {
      searchInput.addEventListener('input', this.debounce(() => this.applyFilters(), 300));
    }
    
    if (tagFilter) {
      tagFilter.addEventListener('change', () => this.applyFilters());
    }
    
    if (methodFilter) {
      methodFilter.addEventListener('change', () => this.applyFilters());
    }
    
    if (clearFiltersBtn) {
      clearFiltersBtn.addEventListener('click', () => this.clearFilters());
    }
    
    // Set up global functions for direct access from HTML
    window.MakerAPI = this;
  },
  
  // Initialize event listeners after the DOM is fully loaded
  setupGlobalEventHandlers() {  
    // Tab switching and button handling (delegated events)
    document.body.addEventListener('click', (e) => {
      
      // Handle tab button clicks
      if (e.target.closest('.endpoint-tab-button')) {
        e.preventDefault();
        const button = e.target.closest('.endpoint-tab-button');
        const routeId = button.closest('.api-endpoint').dataset.routeId;
        const tabId = button.dataset.tab;
        this.switchEndpointTab(routeId, tabId);
      }
      
      // Handle execute test button clicks
      if (e.target.closest('.endpoint-execute-btn')) {
        e.preventDefault();
        const button = e.target.closest('.endpoint-execute-btn');
        const routeId = button.closest('.api-endpoint').dataset.routeId;
        const route = this.getRouteById(routeId);
        if (route) {
          this.executeEndpointTest(routeId, route);
        }
      }
      
      // Handle copy code button clicks
      if (e.target.closest('.copy-code-btn')) {
        e.preventDefault();
        const button = e.target.closest('.copy-code-btn');
        const code = button.dataset.code;
        if (code) {
          this.copyCode(code);
        }
      }
    }, false);
    
    // Make toggleSection available globally for onclick handlers
    window.MakerAPI = this;
  },
  
  // Apply filters to routes
  applyFilters() {
    const searchInput = document.getElementById('route-search');
    const tagFilter = document.getElementById('tag-filter');
    const methodFilter = document.getElementById('method-filter');
    
    const searchTerm = searchInput ? searchInput.value.toLowerCase().trim() : '';
    const selectedTag = tagFilter ? tagFilter.value : '';
    const method = methodFilter ? methodFilter.value : '';
    
    let filtered = this.state.routes.filter(route => {
      // Search filter
      if (searchTerm) {
        const searchText = [
          route.path,
          route.method,
          route.summary || route.description || '',
          route.module || '',
          route.name || '',
          (route.tags || []).join(' ')
        ].join(' ').toLowerCase();
        
        if (!searchText.includes(searchTerm)) return false;
      }
      
      // Tag filter - check if selected tag appears anywhere in route's tags
      if (selectedTag) {
        const routeTags = route.tags || [route.module || 'Platform'];
        const hasTag = routeTags.some(tag => 
          this.formatModuleName(tag) === selectedTag ||
          tag.toLowerCase() === selectedTag.toLowerCase()
        );
        if (!hasTag) return false;
      }
      
      // Method filter
      if (method && route.method.toLowerCase() !== method) return false;
      
      return true;
    });
    
    // Temporarily replace routes for rendering
    const originalRoutes = this.state.routes;
    this.state.routes = filtered;
    this.renderRoutes();
    this.state.routes = originalRoutes;
  },
  
  // Clear all filters
  clearFilters() {
    const elements = [
      'route-search',
      'tag-filter',
      'method-filter'
    ];
    
    elements.forEach(id => {
      const el = document.getElementById(id);
      if (el) el.value = '';
    });
    
    this.renderRoutes();
  },
  
  // Refresh routes
  async refreshRoutes() {
    const btn = document.getElementById('refresh-routes');
    if (btn) {
      btn.disabled = true;
      btn.textContent = '⟳ Refreshing...';
    }
    
    try {
      // Reload configuration first to check if specs changed
      await this.loadOpenApiConfiguration();
      
      // Check if we still have specs available
      if (this.state.availableSpecs.length === 0) {
        this.showNoSpecsMessage();
        return;
      }
      
      // If we had a multi-spec selector but now only have one, or vice versa, we may need to update the UI
      const currentSelector = document.getElementById('spec-selector');
      const shouldHaveSelector = this.state.availableSpecs.length > 1;
      
      if (shouldHaveSelector && !currentSelector) {
        // Need to add selector
        this.setupSpecSelector();
      } else if (!shouldHaveSelector && currentSelector) {
        // Need to remove selector
        const selectorContainer = currentSelector.closest('.spec-selector-container');
        if (selectorContainer) {
          selectorContainer.remove();
        }
      }
      
      // Load routes with current/updated configuration
      await this.loadRoutes();
      
    } catch (error) {
      console.error('Failed to refresh:', error);
      this.showError(`Refresh failed: ${error.message}`);
    }
    
    if (btn) {
      btn.disabled = false;
      btn.textContent = '🔄 Refresh Routes';
    }
  },
  
  // Setup spec selector UI when multiple specs are available
  setupSpecSelector() {
    const pageHeader = document.querySelector('.page-header');
    if (!pageHeader) {
      console.warn('No .page-header found for spec selector');
      return;
    }
    
    // Create top row container if it doesn't exist
    let topRowContainer = document.querySelector('.top-row-container');
    if (!topRowContainer) {
      const topRowHtml = `
        <div class="top-row-container has-spec">
          <div class="spec-quadrant">
            <!-- OpenAPI Specification -->
            <div class="spec-selector-container">
              <div class="form-group">
                <label for="spec-selector" class="quadrant-title">📊 OpenAPI Specification:</label>
                <select id="spec-selector" class="form-control">
                  ${this.state.availableSpecs.map(spec => 
                    `<option value="${spec.id}" ${spec.id === this.state.selectedSpec ? 'selected' : ''}>${spec.name}</option>`
                  ).join('')}
                </select>
                <small>Select which API specification to view and test.</small>
              </div>
            </div>
            
            <!-- API Server Info -->
            <div class="server-info-container">
              <div class="form-group">
                <label class="quadrant-title">🌐 API Server:</label>
                <div class="server-details">
                  <div class="server-item">
                    <span class="label">Base URL:</span>
                    <span class="value">${window.location.origin}</span>
                  </div>
                  <div class="server-item">
                    <span class="label">Spec Version:</span>
                    <span class="value" id="spec-version">Loading...</span>
                  </div>
                  <div class="server-item">
                    <span class="label">Status:</span>
                    <span class="server-status online">🟢 Online</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
          <div class="token-placeholder"></div>
        </div>
      `;
      
      // Insert after the page-header
      pageHeader.insertAdjacentHTML('afterend', topRowHtml);
      topRowContainer = document.querySelector('.top-row-container');
      
      // Move the token section into the top row
      const tokenSection = document.getElementById('token-section');
      const tokenPlaceholder = document.querySelector('.token-placeholder');
      if (tokenSection && tokenPlaceholder) {
        tokenPlaceholder.replaceWith(tokenSection);
        
        // Ensure token section has consistent title styling
        const tokenLabels = tokenSection.querySelectorAll('label');
        tokenLabels.forEach(label => {
          if (!label.classList.contains('quadrant-title')) {
            label.classList.add('quadrant-title');
          }
        });
      }
      
      // Update server info with actual spec data when available
      this.updateServerInfo();
    }
    
    // Setup event listener for spec changes
    const selector = document.getElementById('spec-selector');
    if (selector) {
      selector.addEventListener('change', (e) => this.onSpecSelectionChange(e));
    }
  },
  
  // Handle spec selection change
  async onSpecSelectionChange(event) {
    const newSpec = event.target.value;
    if (newSpec === this.state.selectedSpec) {
      return; // No change
    }
    
    this.state.selectedSpec = newSpec;
    
    // Show loading and reload routes
    this.showLoading(true);
    
    try {
      await this.loadRoutes();
      
      // Update server info with new spec data
      this.updateServerInfo();
      
      const specInfo = this.state.availableSpecs.find(spec => spec.id === newSpec);
      this.showToast(`Switched to ${specInfo ? specInfo.name : 'selected specification'}`, 'success');
    } catch (error) {
      console.error('Failed to switch specification:', error);
      this.showError(`Failed to load ${newSpec} specification: ${error.message}`);
    }
  },
  
  // Update server info section with current spec data
  updateServerInfo() {
    const specVersionEl = document.getElementById('spec-version');
    if (!specVersionEl) return;
    
    if (this.state.openApiSpec && this.state.openApiSpec.info) {
      const version = this.state.openApiSpec.info.version || 'Unknown';
      const title = this.state.openApiSpec.info.title || 'API';
      specVersionEl.textContent = `${title} v${version}`;
    } else {
      specVersionEl.textContent = 'Loading...';
    }
  },
  
  // Show message when no OpenAPI specs are available
  showNoSpecsMessage() {
    const container = document.getElementById('routes-container') || document.querySelector('.container');
    if (!container) {
      console.error('No container found to show no-specs message');
      return;
    }
    
    const messageHtml = `
      <div class="no-specs-message">
        <h2>📋 No OpenAPI Specifications Available</h2>
        <p class="no-specs-description">
          OpenAPI documentation is not currently enabled. To enable API documentation and testing capabilities, compile your project with one or both of the following build flags:
        </p>
        <div class="build-flags">
          <div><strong>Full Platform API:</strong></div>
          <div class="flag-value">-DWEB_PLATFORM_OPENAPI=1</div>
          <div><strong>Maker API Only:</strong></div>
          <div class="flag-value">-DWEB_PLATFORM_MAKERAPI=1</div>
        </div>
        <p class="no-specs-instruction">
          Add these flags to your <code>platformio.ini</code> file under <code>build_flags =</code> and recompile to enable OpenAPI documentation.
        </p>
      </div>
    `;
    
    // Replace the routes container content
    container.innerHTML = messageHtml;
    
    // Hide other UI elements
    this.showLoading(false);
    
    // Update layout for no-spec case - change to single column with token section
    const topRowContainer = document.querySelector('.top-row-container');
    if (topRowContainer) {
      topRowContainer.classList.remove('has-spec');
      topRowContainer.classList.add('no-spec');
      
      // Ensure token section styling is consistent even without spec
      const tokenSection = document.getElementById('token-section');
      if (tokenSection) {
        const tokenLabels = tokenSection.querySelectorAll('label');
        tokenLabels.forEach(label => {
          if (!label.classList.contains('quadrant-title')) {
            label.classList.add('quadrant-title');
          }
        });
      }
    }
    
    const elements = {
      'error-message': 'none',
      'no-routes-message': 'none'
    };
    
    Object.entries(elements).forEach(([id, display]) => {
      const el = document.getElementById(id);
      if (el) el.style.display = display;
    });
  },

  // Download OpenAPI specification
  async downloadOpenAPISpec() {
    try {
      const selectedSpecInfo = this.state.availableSpecs.find(spec => spec.id === this.state.selectedSpec);
      if (!selectedSpecInfo) {
        this.showToast('No specification selected for download', 'error');
        return;
      }
      
      const response = await fetch(selectedSpecInfo.url);
      if (!response.ok) throw new Error('Failed to fetch OpenAPI spec');
      
      const spec = await response.json();
      const blob = new Blob([JSON.stringify(spec, null, 2)], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      
      const link = document.createElement('a');
      link.href = url;
      link.download = `openapi-spec-${this.state.selectedSpec}.json`;
      document.body.appendChild(link);
      link.click();
      
      setTimeout(() => {
        document.body.removeChild(link);
        URL.revokeObjectURL(url);
      }, 100);
      
      this.showToast(`${selectedSpecInfo.name} downloaded`, 'success');
      
    } catch (error) {
      console.error('Download failed:', error);
      this.showToast('Failed to download OpenAPI spec', 'error');
    }
  },
  
  // Load available tokens for the current user
  async loadAvailableTokens() {
    try {
      // Always make sure the selector is initialized first, so UI works regardless
      this.state.availableTokens = [];
      this.updateTokenSelector();
      
      // Try to get the user's tokens if we're logged in
      try {
        // First check if we're logged in by getting current user
        const userResponse = await fetch('/api/user', {
          method: 'GET',
          headers: {
            'Accept': 'application/json',
            'X-Requested-With': 'XMLHttpRequest'
          },
          credentials: 'include'
        });
        
        if (!userResponse.ok) {
          return;
        }
        
        // Parse the user data - the ID might be in different locations depending on API structure
        const userData = await userResponse.json();
        let userId = null;
        
        // Look for ID in different possible locations
        if (userData.id) {
          userId = userData.id;
        } else if (userData.user && userData.user.id) {
          userId = userData.user.id;
        } else if (userData.data && userData.data.id) {
          userId = userData.data.id;
        }
        
        // Try to use the userData object directly if we couldn't find an ID
        if (!userId && userData) {
          
          // Try the /api/tokens endpoint first as fallback
          const tokensResponse = await fetch('/api/tokens', {
            method: 'GET',
            headers: {
              'Accept': 'application/json',
              'X-Requested-With': 'XMLHttpRequest'
            },
            credentials: 'include'
          });
          
          if (tokensResponse.ok) {
            const tokensData = await tokensResponse.json();
            if (tokensData.tokens && tokensData.tokens.length > 0) {
              this.state.availableTokens = tokensData.tokens;
              this.updateTokenSelector();
              return;
            }
          }
          
          return;
        }
        
        
        // Now fetch tokens for this user
        const tokensResponse = await fetch(`/api/users/${userId}/tokens`, {
          method: 'GET',
          headers: {
            'Accept': 'application/json',
            'X-Requested-With': 'XMLHttpRequest'
          },
          credentials: 'include'
        });
        
        if (tokensResponse.ok) {
          const tokensData = await tokensResponse.json();
          this.state.availableTokens = tokensData.tokens || [];
          this.updateTokenSelector();
        } else {
          console.warn(`Failed to load tokens: ${tokensResponse.status} ${tokensResponse.statusText}`);
        }
      } catch (innerError) {
        console.warn('Error while trying to load tokens:', innerError);
      }
    } catch (error) {
      console.warn('Failed to load available tokens (outer error):', error);
    }
  },
  
  // Update the token selector dropdown
  updateTokenSelector() {
    const tokenSelector = document.getElementById('token-selector');
    const tokenInput = document.getElementById('api-token-input');
    
    if (!tokenSelector) return;
    
    // Clear existing options
    tokenSelector.innerHTML = '<option value="manual">Enter token manually</option>';
    
    // Add existing tokens
    if (this.state.availableTokens && this.state.availableTokens.length > 0) {
      this.state.availableTokens.forEach(token => {
        const option = document.createElement('option');
        option.value = token.value || token.token;  // Handle different API formats
        option.textContent = `🔑 ${token.name}${token.description ? ` - ${token.description}` : ''}`;
        tokenSelector.appendChild(option);
      });
    }
    
    // Set default to manual entry
    tokenSelector.value = 'manual';
    
    // Initial state setup
    if (tokenInput) {
      tokenInput.disabled = false;
      tokenInput.placeholder = 'Enter your API token here...';
    }
    
    // Update CSS to properly format the token section
    this.updateTokenSectionStyles();
  },
  
  // Update token section - styles now handled in CSS file
  updateTokenSectionStyles() {
    // Token section styling is now handled in maker_api_styles_css.h
    // This function remains for any future dynamic styling needs
  },
  
  // Handle token selection change
  onTokenSelectionChange(event) {
    const tokenInput = document.getElementById('api-token-input');
    if (!tokenInput) return;
    
    const selectedValue = event.target.value;
    
    if (selectedValue === 'manual') {
      // Enable manual entry
      tokenInput.disabled = false;
      tokenInput.value = '';
      tokenInput.placeholder = 'Enter your API token here...';
      this.state.token = null;
      this.showToast('Enter your token manually', 'info');
    } else {
      // Use selected token
      tokenInput.disabled = true;
      tokenInput.value = selectedValue;
      tokenInput.placeholder = '';
      this.state.token = selectedValue;
      
      // Find the token name from our list
      const selectedToken = this.state.availableTokens.find(t => 
        (t.value && t.value === selectedValue) || (t.token && t.token === selectedValue)
      );
      const tokenName = selectedToken ? selectedToken.name : 'Selected token';
      
      this.showToast(`Using token: ${tokenName}`, 'success');
    }
  },
  

  
  // Create parameter input field
  createParameterInput(param) {
    if (!param || !param.name) {
      console.error('Invalid parameter object:', param);
      return '<div class="parameter-input">Invalid parameter</div>';
    }
    
    const required = param.required ? ' *' : '';
    const type = param.schema && param.schema.type ? param.schema.type : 'string';
    const paramName = this.escapeHtml(param.name);
    const paramDesc = param.description ? this.escapeHtml(param.description) : '';
    
    let inputHtml = '';
    switch (type) {
      case 'boolean':
        inputHtml = `
          <select name="${paramName}" ${param.required ? 'required' : ''}>
            <option value="">Select...</option>
            <option value="true">true</option>
            <option value="false">false</option>
          </select>
        `;
        break;
      case 'integer':
      case 'number':
        inputHtml = `<input type="number" name="${paramName}" ${param.required ? 'required' : ''} 
                     placeholder="${paramDesc || paramName}">`;
        break;
      default:
        inputHtml = `<input type="text" name="${paramName}" ${param.required ? 'required' : ''} 
                     placeholder="${paramDesc || paramName}">`;
        break;
    }
    
    return `
      <div class="parameter-input">
        <label>
          ${paramName}${required}
          ${paramDesc ? `<span class="param-description">${paramDesc}</span>` : ''}
        </label>
        ${inputHtml}
      </div>
    `;
  },

  // Generate unique route ID
  generateRouteId(route) {
    return `route-${route.method.toLowerCase()}-${route.path.replace(/[^a-zA-Z0-9]/g, '-')}`;
  },
  
  // Get route by ID
  getRouteById(routeId) {
    return this.state.routes.find(route => this.generateRouteId(route) === routeId);
  },
  
  // Toggle endpoint expansion
  toggleEndpoint(routeId) {
    const endpoint = document.querySelector(`[data-route-id="${routeId}"]`);
    if (!endpoint) {
      console.error(`Endpoint not found: ${routeId}`);
      return;
    }
    
    const isExpanded = endpoint.classList.contains('expanded');
    
    // Collapse all other endpoints
    document.querySelectorAll('.api-endpoint.expanded').forEach(el => {
      if (el !== endpoint) {
        el.classList.remove('expanded');
      }
    });
    
    // Toggle current endpoint
    if (isExpanded) {
      endpoint.classList.remove('expanded');
    } else {
      endpoint.classList.add('expanded');
      
      // Get active tab button if any
      const activeTabButton = endpoint.querySelector('.endpoint-tab-button.active');
      const tabId = activeTabButton ? activeTabButton.dataset.tab : 'try';
      
      // Initialize the appropriate tab (either active or default to 'try')
      this.switchEndpointTab(routeId, tabId);
    }
  },
  
  // Switch endpoint tab
  switchEndpointTab(routeId, tabId) {
    const endpoint = document.querySelector(`[data-route-id="${routeId}"]`);
    if (!endpoint) {
      console.error(`Endpoint with ID ${routeId} not found`);
      return;
    }
    
    // Update tab buttons
    const tabButtons = endpoint.querySelectorAll('.endpoint-tab-button');
    const tabPanels = endpoint.querySelectorAll('.endpoint-tab-panel');
        
    tabButtons.forEach(btn => {
      btn.classList.toggle('active', btn.dataset.tab === tabId);
    });
    
    tabPanels.forEach(panel => {
      const isActive = panel.id === `${routeId}-${tabId}-tab`;
      panel.classList.toggle('active', isActive);
    });
    
    // If switching to cURL tab, immediately generate the command
    if (tabId === 'curl') {
      // Force immediate generation
      this.generateCurlCommand(routeId);
    }
  },
  
  // Render endpoint content with tabs
  renderEndpointContent(route) {
    const routeId = this.generateRouteId(route);
    
    return `
      <div class="endpoint-tabs">
        <div class="endpoint-tab-buttons">
          <button type="button" class="endpoint-tab-button active" data-tab="try" onclick="MakerAPI.switchEndpointTab('${routeId}', 'try')">🧪 Try It</button>
          <button type="button" class="endpoint-tab-button" data-tab="curl" onclick="MakerAPI.switchEndpointTab('${routeId}', 'curl')">💻 cURL</button>
          ${this.state.selectedSpec === 'full' ? `<button type="button" class="endpoint-tab-button" data-tab="disable" onclick="MakerAPI.switchEndpointTab('${routeId}', 'disable')">🚫 Disable</button>` : ''}
          ${this.state.selectedSpec === 'full' ? `<button type="button" class="endpoint-tab-button" data-tab="override" onclick="MakerAPI.switchEndpointTab('${routeId}', 'override')">🔄 Override</button>` : ''}
          <button type="button" class="endpoint-tab-button" data-tab="details" onclick="MakerAPI.switchEndpointTab('${routeId}', 'details')">📋 Details</button>
        </div>
        
        <div class="endpoint-tab-content">
          <div class="endpoint-tab-panel active" id="${routeId}-try-tab">
            ${this.renderTryItTab(route, routeId)}
          </div>
          
          <div class="endpoint-tab-panel" id="${routeId}-curl-tab">
            ${this.renderCurlTab(route, routeId)}
          </div>
          
          ${this.state.selectedSpec === 'full' ? `<div class="endpoint-tab-panel" id="${routeId}-disable-tab">
            ${this.renderDisableTab(route)}
          </div>` : ''}
          
          ${this.state.selectedSpec === 'full' ? `<div class="endpoint-tab-panel" id="${routeId}-override-tab">
            ${this.renderOverrideTab(route)}
          </div>` : ''}
          
          <div class="endpoint-tab-panel" id="${routeId}-details-tab">
            ${this.renderDetailsTab(route)}
          </div>
        </div>
      </div>
    `;
  },
  
  // Render Try It tab
  renderTryItTab(route, routeId) {
    const parameters = this.getRouteParameters(route);
    const hasRequestBody = this.hasRequestBody(route);
    const requestBodyTemplate = this.getRequestBodyTemplate(route);
    
    // Get authentication types - check both authTypes array and fallback to authType
    const authTypes = route.authTypes || [route.authType || route.auth || 'none'];
    let authSelector = '';
    
    // Show auth selector if multiple auth types or if not 'none'
    if (authTypes.length > 1 || (authTypes.length === 1 && authTypes[0] !== 'none')) {
      const options = authTypes.map(authType => {
        const label = this.getAuthLabel(authType);
        const icon = this.getAuthIcon(authType);
        let description = '';
        
        switch(authType) {
          case 'session':
            description = ' (Current Login)';
            break;
          case 'token':
            description = ' (API Token from token section)';
            break;
          case 'local_only':
            description = ' (Local Network Only)';
            break;
          case 'none':
            description = ' (No authentication required)';
            break;
        }
        
        return `<option value="${authType}">${icon} ${label}${description}</option>`;
      }).join('');
      
      // Default to first available auth type
      const defaultAuth = authTypes.includes('session') ? 'session' : authTypes[0];
      
      authSelector = `
        <div class="auth-selector">
          <h4>${authTypes.includes('token') ? '🔑 ' : ''}Authentication Method</h4>
          <select id="${routeId}-auth-type" class="form-control">
            ${options}
          </select>
          ${authTypes.includes('token') ? `
            <div class="auth-token-info">
              <small>💡 <strong>Token authentication:</strong> Uses the token from the "API Token" section above.</small>
            </div>
          ` : ''}
        </div>
      `;
    }
    
    return `
      <div class="endpoint-try-section">
        ${authSelector}
        
        ${parameters && parameters.length > 0 ? `
        <div class="endpoint-parameter-section">
          <h4>Parameters</h4>
          <form id="${routeId}-form">
            ${parameters.map(param => this.createParameterInput(param)).join('')}
          </form>
        </div>
        ` : ''}
        
        ${hasRequestBody ? `
        <div class="endpoint-body-section">
          <h4>Request Body</h4>
          <div class="request-body-controls">
            <label for="${routeId}-body">JSON Body:</label>
            <textarea id="${routeId}-body" class="form-control code-input" rows="8" placeholder="Enter JSON request body...">${this.escapeHtml(requestBodyTemplate)}</textarea>
          </div>
        </div>
        ` : ''}
        
        ${!parameters || parameters.length === 0 ? !hasRequestBody ? '<p>This endpoint requires no parameters or request body.</p>' : '' : ''}
        
        <div class="endpoint-test-actions">
          <button type="button" class="btn btn-primary endpoint-execute-btn" onclick="MakerAPI.executeEndpointTest('${routeId}', MakerAPI.getRouteById('${routeId}'))">Try It!</button>
        </div>
        
        <div class="endpoint-test-results" id="${routeId}-results">
          <h4>Response</h4>
          <div class="response-status" id="${routeId}-status"></div>
          <div class="response-body" id="${routeId}-body"></div>
        </div>
      </div>
    `;
  },
  
  // Render Disable tab
  renderDisableTab(route) {
    const disableCode = `// This will disable ${route.path} by deregistering it\nwebPlatform.registerApiRoute("${route.path.replace('/api','')}", nullptr);`;
    const escapedCode = this.escapeHtml(disableCode);
    
    return `
      <h4>Disable This Route</h4>
      <p>Completely disables this route by deregistering it - returns 404</p>
      <p><strong>Usage:</strong> Add this line in your setup() function after webPlatform.begin() and any registerModule calls.</p>
      
      <div class="code-section">
        <div class="code-header">
          <button type="button" class="btn btn-small btn-primary copy-code-btn" data-code="${escapedCode}" onclick="MakerAPI.copyCode(this.dataset.code)">
            📋 Copy Code
          </button>
        </div>
        <pre class="code-block"><code>${escapedCode}</code></pre>
      </div>
    `;
  },
  
  // Render Override tab
  renderOverrideTab(route) {
    const overrideCode = `// Override route: ${route.method.toUpperCase()} ${route.path}\nwebPlatform.registerApiRoute("${route.path.replace('/api','')}", customHandler, {AuthType::NONE});\n\n// Implement your custom handler:\nvoid customHandler(WebRequest& req, WebResponse& res) {\n  res.setContent("Custom response", "text/plain");\n}`;
    const escapedCode = this.escapeHtml(overrideCode);
    
    return `
      <h4>Override This Route</h4>
      <p>Replaces route with custom handler by re-registering it</p>
      <p><strong>Usage:</strong> Add this line and implement your custom handler function after webPlatform.begin() and any registerModule calls.</p>
      
      <div class="code-section">
        <div class="code-header">
          <button type="button" class="btn btn-small btn-primary copy-code-btn" data-code="${escapedCode}" onclick="MakerAPI.copyCode(this.dataset.code)">
            📋 Copy Code
          </button>
        </div>
        <pre class="code-block"><code>${escapedCode}</code></pre>
      </div>
    `;
  },
  
  // Render Details tab
  renderDetailsTab(route) {
    return `
      <h4>Route Information</h4>
      <div class="route-details">
        <div class="detail-item"><strong>Path:</strong> ${this.escapeHtml(route.path)}</div>
        <div class="detail-item"><strong>Method:</strong> ${route.method.toUpperCase()}</div>
        <div class="detail-item"><strong>Authentication:</strong> ${this.getAuthLabel(route.authType || route.auth)}</div>
        ${route.module ? `<div class="detail-item"><strong>Module:</strong> ${this.escapeHtml(route.module)}</div>` : ''}
        ${route.summary ? `<div class="detail-item"><strong>Summary:</strong> ${this.escapeHtml(route.summary)}</div>` : ''}
        ${route.description ? `<div class="detail-item"><strong>Description:</strong> ${this.escapeHtml(route.description)}</div>` : ''}
        ${route.operationId ? `<div class="detail-item"><strong>Operation ID:</strong> ${this.escapeHtml(route.operationId)}</div>` : ''}
        ${route.tags && route.tags.length > 0 ? `<div class="detail-item"><strong>Tags:</strong> ${route.tags.map(tag => this.escapeHtml(tag)).join(', ')}</div>` : ''}
      </div>
    `;
  },
  
  // Get route parameters from OpenAPI spec
  getRouteParameters(route) {
    // Try to get parameters from route object first
    if (route.parameters && Array.isArray(route.parameters)) {
      return route.parameters;
    }
    
    // Fall back to looking in OpenAPI spec
    if (!this.state.openApiSpec || !this.state.openApiSpec.paths) {
      return [];
    }
    
    const pathInfo = this.state.openApiSpec.paths[route.path];
    if (!pathInfo) {
      return [];
    }
    
    const methodInfo = pathInfo[route.method.toLowerCase()];
    if (!methodInfo) {
      return [];
    }
    
    return methodInfo.parameters || [];
  },
  
  // Check if route has request body (POST/PUT/PATCH methods with body)
  hasRequestBody(route) {
    const method = route.method.toUpperCase();
    return ['POST', 'PUT', 'PATCH'].includes(method) && this.getRequestBodyFromSpec(route);
  },
  
  // Get request body template from OpenAPI spec
  getRequestBodyTemplate(route) {
    const requestBody = this.getRequestBodyFromSpec(route);
    if (!requestBody) {
      return '';
    }
    
    // Try to get example from the spec first
    if (requestBody.content && requestBody.content['application/json']) {
      const jsonContent = requestBody.content['application/json'];
      
      // Use example if available
      if (jsonContent.example) {
        return JSON.stringify(jsonContent.example, null, 2);
      }
      
      // Try to generate example from schema
      if (jsonContent.schema) {
        const example = this.generateExampleFromSchema(jsonContent.schema);
        if (example) {
          return JSON.stringify(example, null, 2);
        }
      }
    }
    
    // Default empty JSON object
    return '{\n  \n}';
  },
  
  // Get request body from OpenAPI spec
  getRequestBodyFromSpec(route) {
    if (!this.state.openApiSpec || !this.state.openApiSpec.paths) {
      return null;
    }
    
    const pathInfo = this.state.openApiSpec.paths[route.path];
    if (!pathInfo) {
      return null;
    }
    
    const methodInfo = pathInfo[route.method.toLowerCase()];
    if (!methodInfo || !methodInfo.requestBody) {
      return null;
    }
    
    return methodInfo.requestBody;
  },
  
  // Generate example JSON from OpenAPI schema
  generateExampleFromSchema(schema) {
    if (!schema || typeof schema !== 'object') {
      return null;
    }
    
    try {
      if (schema.type === 'object' && schema.properties) {
        const example = {};
        
        for (const [propName, propSchema] of Object.entries(schema.properties)) {
          if (propSchema.type === 'string') {
            // Use description as example if available
            if (propSchema.description) {
              example[propName] = propSchema.description.includes('password') ? 'your_password_here' :
                                propSchema.description.includes('username') ? 'your_username_here' :
                                propSchema.description.includes('name') ? 'example_name' :
                                propSchema.description.includes('token') ? 'your_token_name' :
                                'example_value';
            } else {
              example[propName] = 'string_value';
            }
          } else if (propSchema.type === 'integer') {
            example[propName] = 0;
          } else if (propSchema.type === 'number') {
            example[propName] = 0.0;
          } else if (propSchema.type === 'boolean') {
            example[propName] = false;
          } else if (propSchema.type === 'array') {
            example[propName] = [];
          } else {
            example[propName] = null;
          }
        }
        
        return example;
      }
    } catch (error) {
      console.warn('Failed to generate example from schema:', error);
    }
    
    return null;
  },
  
  // Execute endpoint test directly without proxy
  async executeEndpointTest(routeId, route) {  
    const executeBtn = document.querySelector(`[data-route-id="${routeId}"] .endpoint-execute-btn`);
    const resultsEl = document.getElementById(`${routeId}-results`);
    const statusEl = document.getElementById(`${routeId}-status`);
    const bodyEl = document.getElementById(`${routeId}-body`);
    
    if (!executeBtn || !resultsEl || !statusEl || !bodyEl) {
      console.error(`Missing UI elements for route ${routeId}`);
      return;
    }
    
    try {
      executeBtn.textContent = 'Executing...';
      executeBtn.disabled = true;
      
      // Collect parameters from form
      const form = document.getElementById(`${routeId}-form`);
      const paramValues = {};
      
      if (form) {
        const inputs = form.querySelectorAll('input, select');
        inputs.forEach(input => {
          if (input.value) {
            let value = input.value;
            if (input.type === 'number') {
              value = parseFloat(value);
            } else if (value === 'true') {
              value = true;
            } else if (value === 'false') {
              value = false;
            }
            paramValues[input.name] = value;
          }
        });
      }
      
      // Get request body if present
      let requestBodyData = null;
      const bodyTextarea = document.getElementById(`${routeId}-body`);
      if (bodyTextarea && bodyTextarea.value && bodyTextarea.value.trim) {
        const bodyValue = bodyTextarea.value.trim();
        if (bodyValue) {
          try {
            requestBodyData = JSON.parse(bodyValue);
          } catch (error) {
            resultsEl.classList.add('show');
            statusEl.innerHTML = '<span class="error">❌ Invalid JSON in request body</span>';
            bodyEl.innerHTML = `<pre><code>JSON Parse Error: ${error.message}</code></pre>`;
            return;
          }
        }
      }
            
      // Get auth type - check if there's a selector for auth type choice
      let authType = route.authType || route.auth || 'none';
      const authSelector = document.getElementById(`${routeId}-auth-type`);
      if (authSelector) {
        // Use selected auth type from dropdown
        authType = authSelector.value;
      } else if (route.authTypes && route.authTypes.length === 1) {
        // Use the single auth type if only one available
        authType = route.authTypes[0];
      }
      
      // Create request options
      const options = {
        method: route.method.toUpperCase(),
        // Set credentials to 'omit' when using token auth to prevent sending cookies
        credentials: authType === 'token' ? 'omit' : 'include',
        headers: {
          'Accept': 'application/json',
          'X-Requested-With': 'XMLHttpRequest'
        }
      };
      
      // Apply authentication based on type
      if (authType === 'token') {
        // Use token from the token section - even if empty
        const tokenInput = document.getElementById('api-token-input');
        const tokenValue = (tokenInput && tokenInput.value) ? tokenInput.value.trim() : '';
        options.headers['Authorization'] = `Bearer ${tokenValue}`;
      }
      
      // Handle CSRF token for session-based requests
      if (authType === 'session') {
        const csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute('content');
        if (csrfToken) {
          options.headers['X-CSRF-TOKEN'] = csrfToken;
        }
      }
      
      // Build URL with path parameters replaced and query parameters
      let url = route.path;
      const usedParams = new Set();
      
      // First, replace path parameters (like {id})
      Object.entries(paramValues).forEach(([key, value]) => {
        const placeholder = `{${key}}`;
        if (url.includes(placeholder)) {
          url = url.replace(placeholder, encodeURIComponent(value));
          usedParams.add(key);
        }
      });
      
      // Then add remaining parameters as query parameters for GET requests
      if (route.method.toUpperCase() === 'GET') {
        const queryParams = Object.entries(paramValues)
          .filter(([key]) => !usedParams.has(key));
          
        if (queryParams.length > 0) {
          const queryString = queryParams
            .map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`)
            .join('&');
          url = `${url}${url.includes('?') ? '&' : '?'}${queryString}`;
        }
      }
      
      // Add request body for non-GET methods
      if (route.method.toUpperCase() !== 'GET') {
        let bodyToSend = null;
        
        // Prioritize manual request body from textarea
        if (requestBodyData) {
          bodyToSend = requestBodyData;
        } else {
          // Fallback to form parameters (excluding path parameters)
          const bodyParams = Object.entries(paramValues)
            .filter(([key]) => !usedParams.has(key))
            .reduce((obj, [key, value]) => {
              obj[key] = value;
              return obj;
            }, {});
            
          if (Object.keys(bodyParams).length > 0) {
            bodyToSend = bodyParams;
          }
        }
        
        if (bodyToSend) {
          options.headers['Content-Type'] = 'application/json';
          options.body = JSON.stringify(bodyToSend);
        }
      }
      
      // Keep track of request details for display
      const requestDetails = {
        url: url,
        method: options.method,
        headers: {...options.headers},
        body: options.body || null,
        bodyData: requestBodyData // Store parsed body data for display
      };
            
      // Execute direct request
      const startTime = Date.now();
      const response = await fetch(url, options);
      const endTime = Date.now();
      const duration = endTime - startTime;
      
      // Parse response
      let responseData;
      let responseText;
      const contentType = response.headers.get('content-type') || '';
      
      if (contentType.includes('application/json')) {
        responseData = await response.json();
        responseText = JSON.stringify(responseData, null, 2);
      } else {
        responseText = await response.text();
        try {
          // Try to parse as JSON anyway in case content-type is wrong
          responseData = JSON.parse(responseText);
          responseText = JSON.stringify(responseData, null, 2);
        } catch (e) {
          // Keep as text if not valid JSON
          responseData = { text: responseText };
        }
      }
      
      // Format response headers
      const responseHeaders = {};
      response.headers.forEach((value, key) => {
        responseHeaders[key] = value;
      });
      
      // Show results
      resultsEl.classList.add('show');
      
      const isSuccess = response.ok;
      const statusClass = isSuccess ? 'success' : 'error';
      
      statusEl.innerHTML = `<span class="${statusClass}">
        ${isSuccess ? '✅' : '❌'} Status: ${response.status} ${response.statusText} (${duration}ms)
      </span>`;
      
      // Display both request and response details
      bodyEl.innerHTML = `
        <h5>📤 Request</h5>
        <div class="request-details">
          <p><strong>URL:</strong> ${requestDetails.url}</p>
          <p><strong>Method:</strong> ${requestDetails.method}</p>
          <p><strong>Headers:</strong></p>
          <pre><code>${JSON.stringify(requestDetails.headers, null, 2)}</code></pre>
          ${requestDetails.body ? `
            <p><strong>Body:</strong></p>
            <pre><code>${requestDetails.bodyData ? JSON.stringify(requestDetails.bodyData, null, 2) : requestDetails.body}</code></pre>
          ` : ''}
        </div>
        
        <h5>📥 Response</h5>
        <div class="response-details">
          <p><strong>Status:</strong> ${response.status} ${response.statusText}</p>
          <p><strong>Time:</strong> ${duration}ms</p>
          <p><strong>Headers:</strong></p>
          <pre><code>${JSON.stringify(responseHeaders, null, 2)}</code></pre>
          <p><strong>Body:</strong></p>
          <pre><code>${this.escapeHtml(responseText)}</code></pre>
        </div>
      `;
      
    } catch (error) {
      console.error('API test failed:', error);
      resultsEl.classList.add('show');
      statusEl.innerHTML = '<span class="error">❌ Test failed</span>';
      bodyEl.innerHTML = `<pre><code>Error: ${error.message}</code></pre>`;
    } finally {
      executeBtn.textContent = 'Try It!';
      executeBtn.disabled = false;
    }
  },
  
  // Render cURL tab
  renderCurlTab(route, routeId) {
    const parameters = this.getRouteParameters(route);
    const hasRequestBody = this.hasRequestBody(route);
    const requestBodyTemplate = this.getRequestBodyTemplate(route);
    
    // Get authentication types - same logic as Try It tab
    const authTypes = route.authTypes || [route.authType || route.auth || 'none'];
    let authSelector = '';
    
    // Show auth selector if multiple auth types or if not 'none'
    if (authTypes.length > 1 || (authTypes.length === 1 && authTypes[0] !== 'none')) {
      const options = authTypes.map(authType => {
        const label = this.getAuthLabel(authType);
        const icon = this.getAuthIcon(authType);
        let description = '';
        
        switch(authType) {
          case 'session':
            description = ' (Session Cookie)';
            break;
          case 'token':
            description = ' (API Token)';
            break;
          case 'local_only':
            description = ' (Local Network Only)';
            break;
          case 'none':
            description = ' (No authentication)';
            break;
        }
        
        return `<option value="${authType}">${icon} ${label}${description}</option>`;
      }).join('');
      
      authSelector = `
        <div class="auth-selector">
          <h4>🔧 Authentication Method for cURL</h4>
          <select id="${routeId}-curl-auth-type" class="form-control" onchange="MakerAPI.generateCurlCommand('${routeId}')">
            ${options}
          </select>
        </div>
      `;
    }
    
    return `
      <div class="endpoint-curl-section">
        <h4>💻 cURL Command</h4>
        <p>Configure parameters and authentication method. The cURL command updates automatically.</p>
        
        ${authSelector}
        
        ${parameters && parameters.length > 0 ? `
        <div class="endpoint-parameter-section">
          <h4>Parameters</h4>
          <form id="${routeId}-curl-form">
            ${parameters.map(param => this.createParameterInputForCurl(param, routeId)).join('')}
          </form> 
        </div>
        ` : ''}
        
        <div class="endpoint-curl-actions">
          <button type="button" class="btn btn-secondary" onclick="MakerAPI.copyCurlCommand('${routeId}')">📋 Copy to Clipboard</button>
        </div>
        
        <div class="endpoint-test-results show" id="${routeId}-curl-results">
          <h4>cURL Command</h4>
          <div class="response-status" id="${routeId}-curl-status">
            <span class="success">✅ cURL Command</span>
          </div>
          <div class="response-body" id="${routeId}-curl-output">
            <pre><code>Generating cURL command...</code></pre>
          </div>
        </div>
        
        <script>
          setTimeout(function() { 
            MakerAPI.generateCurlCommand('${routeId}'); 
          }, 50);
        </script>
      </div>
    `;
  },
  
  // Create parameter input for cURL tab
  createParameterInputForCurl(param, routeId) {
    if (!param || !param.name) {
      console.error('Invalid parameter object:', param);
      return '<div class="parameter-input">Invalid parameter</div>';
    }
    
    const required = param.required ? ' *' : '';
    const type = param.schema && param.schema.type ? param.schema.type : 'string';
    const paramName = this.escapeHtml(param.name);
    const paramDesc = param.description ? this.escapeHtml(param.description) : '';
    
    let inputHtml = '';
    switch (type) {
      case 'boolean':
        inputHtml = `
          <select name="${paramName}" onchange="MakerAPI.generateCurlCommand('${routeId}')" ${param.required ? 'required' : ''}>
            <option value="">Select...</option>
            <option value="true">true</option>
            <option value="false">false</option>
          </select>
        `;
        break;
      case 'integer':
      case 'number':
        inputHtml = `<input type="number" name="${paramName}" oninput="MakerAPI.generateCurlCommand('${routeId}')" ${param.required ? 'required' : ''} 
                     placeholder="${paramDesc || paramName}">`;
        break;
      default:
        inputHtml = `<input type="text" name="${paramName}" oninput="MakerAPI.generateCurlCommand('${routeId}')" ${param.required ? 'required' : ''} 
                     placeholder="${paramDesc || paramName}">`;
        break;
    }
    
    return `
      <div class="parameter-input">
        <label>
          ${paramName}${required}
          ${paramDesc ? `<span class="param-description">${paramDesc}</span>` : ''}
        </label>
        ${inputHtml}
      </div>
    `;
  },
  
  // Generate cURL command for the cURL tab
  generateCurlCommand(routeId) {
    const route = this.getRouteById(routeId);
    if (!route) {
      console.error(`Route not found: ${routeId}`);
      return;
    }
    
    // Collect current parameter values from cURL form
    const form = document.getElementById(`${routeId}-curl-form`);
    const paramValues = {};
    
    if (form) {
      const inputs = form.querySelectorAll('input, select');
      inputs.forEach(input => {
        if (input.value && input.value.trim) {
          let value = input.value.trim();
          if (value) {
            if (input.type === 'number') {
              value = parseFloat(value);
            } else if (value === 'true') {
              value = true;
            } else if (value === 'false') {
              value = false;
            }
            paramValues[input.name] = value;
          }
        }
      });
    }
  
    // Build cURL command
    let curl = `curl -X ${route.method.toUpperCase()}`;
    
    // Build URL with path parameters replaced
    let urlPath = route.path;
    const usedParams = new Set();
    
    // Replace path parameters (like {id})
    Object.entries(paramValues).forEach(([key, value]) => {
      const placeholder = `{${key}}`;
      if (urlPath.includes(placeholder)) {
        urlPath = urlPath.replace(placeholder, encodeURIComponent(value));
        usedParams.add(key);
      }
    });
    
    let url = `"${window.location.origin}${urlPath}"`;
    
    // Add remaining parameters as query parameters for GET requests
    if (route.method.toUpperCase() === 'GET') {
      const queryParams = Object.entries(paramValues)
        .filter(([key]) => !usedParams.has(key));
        
      if (queryParams.length > 0) {
        const queryString = queryParams
          .map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`)
          .join('&');
        url = `"${window.location.origin}${urlPath}?${queryString}"`;
      }
    }
    
    curl += ` ${url}`;
    
    // Add headers based on auth type
    let authType = route.authType || route.auth || 'none';
    const authSelector = document.getElementById(`${routeId}-curl-auth-type`);
    if (authSelector) {
      authType = authSelector.value;
    } else if (route.authTypes && route.authTypes.length === 1) {
      authType = route.authTypes[0];
    }
    
    if (authType === 'token') {
      const tokenInput = document.getElementById('api-token-input');
      const tokenValue = (tokenInput && tokenInput.value) ? tokenInput.value.trim() : '';
      curl += ` \\\\\n  -H "Authorization: Bearer ${tokenValue}"`;
      curl += `\n  # Note: This curl command will not send cookies, only the token header`;
      if (!tokenValue) {
        curl += `\n  # Note: Token is empty - select or enter token in "API Token" section above`;
      }
    } else if (authType === 'session') {
      const csrfToken = document.querySelector('meta[name="csrf-token"]')?.getAttribute('content');
      if (csrfToken) {
        curl += ` \\\\\n  -H "X-CSRF-TOKEN: ${csrfToken}"`;
        curl += ` \\\\\n  -H "X-Requested-With: XMLHttpRequest"`;
        curl += ` \\\\\n  --cookie "your_session_cookie_here"`;
        curl += `\n  # Note: You'll need to manually add your session cookie`;
      }
    } else if (authType === 'local_only') {
      curl += `\n  # Note: This endpoint is restricted to local network access`;
    }
    
    // Add body for POST/PUT/PATCH
    if (['POST', 'PUT', 'PATCH'].includes(route.method.toUpperCase())) {
      let bodyToSend = null;
      
      // Check for manual request body first
      const bodyTextarea = document.getElementById(`${routeId}-curl-body`);
      if (bodyTextarea && bodyTextarea.value && bodyTextarea.value.trim) {
        const bodyValue = bodyTextarea.value.trim();
        if (bodyValue) {
          try {
            bodyToSend = JSON.parse(bodyValue);
          } catch (error) {
            // Show error but continue with raw text
            curl += `\n  # Warning: Request body is not valid JSON`;
            curl += ` \\\\\n  -H "Content-Type: application/json"`;
            curl += ` \\\\\n  -d '${bodyValue}'`;
            this.displayCurlCommand(routeId, curl, true);
            return;
          }
        }
      } else {
        // Fallback to form parameters (excluding path parameters)
        const bodyParams = Object.entries(paramValues)
          .filter(([key]) => !usedParams.has(key))
          .reduce((obj, [key, value]) => {
            obj[key] = value;
            return obj;
          }, {});
          
        if (Object.keys(bodyParams).length > 0) {
          bodyToSend = bodyParams;
        }
      }
      
      if (bodyToSend) {
        curl += ` \\\\\n  -H "Content-Type: application/json"`;
        curl += ` \\\\\n  -d '${JSON.stringify(bodyToSend)}'`;
      }
    }
    
    // Display the command
    this.displayCurlCommand(routeId, curl);
  },
  
  // Display cURL command in the results area
  displayCurlCommand(routeId, curl, hasWarning = false) {
    const outputEl = document.getElementById(`${routeId}-curl-output`);
    const statusEl = document.getElementById(`${routeId}-curl-status`);
    
    if (!outputEl) {
      console.error(`curl-output element not found for ${routeId}`);
      return;
    }
    
    if (!statusEl) {
      console.error(`curl-status element not found for ${routeId}`);
    }
    
    const statusClass = hasWarning ? 'warning' : 'success';
    const statusIcon = hasWarning ? '⚠️' : '✅';
    
    // Update status if element exists
    if (statusEl) {
      statusEl.innerHTML = `<span class="${statusClass}">${statusIcon} cURL Command${hasWarning ? ' (with warnings)' : ''}</span>`;
    }
    
    // Update command display
    outputEl.innerHTML = `<pre><code>${this.escapeHtml(curl)}</code></pre>`;
    
    // Store the command for copying
    this.currentCurlCommands = this.currentCurlCommands || {};
    this.currentCurlCommands[routeId] = curl;
    
  },
  
  // Copy current cURL command to clipboard
  copyCurlCommand(routeId) {
    const curlCommand = this.currentCurlCommands && this.currentCurlCommands[routeId];
    
    if (!curlCommand) {
      this.showToast('No cURL command available to copy.', 'warning');
      return;
    }
    
    navigator.clipboard.writeText(curlCommand).then(() => {
      this.showToast('cURL command copied to clipboard', 'success');
    }).catch(err => {
      console.error('Copy failed:', err);
      this.showToast('Failed to copy to clipboard', 'error');
      
      // Fallback for browsers that don't support clipboard API
      const textarea = document.createElement('textarea');
      textarea.value = curlCommand;
      textarea.style.position = 'fixed';
      textarea.style.opacity = '0';
      document.body.appendChild(textarea);
      textarea.select();
      
      try {
        const successful = document.execCommand('copy');
        if (successful) {
          this.showToast('cURL command copied to clipboard (fallback method)', 'success');
        } else {
          this.showToast('Copy failed with fallback method', 'error');
        }
      } catch (err) {
        console.error('Fallback copy failed:', err);
        this.showToast('Failed to copy cURL command', 'error');
      }
      
      document.body.removeChild(textarea);
    });
  },
  
  // Copy code to clipboard
  copyCode(code) {
    if (!code) {
      console.error('No code provided to copy');
      this.showToast('Failed to copy: No code provided', 'error');
      return;
    }
    
    // Decode HTML entities in the code
    const decodedCode = code.replace(/&amp;/g, '&')
                            .replace(/&lt;/g, '<')
                            .replace(/&gt;/g, '>')
                            .replace(/&quot;/g, '"')
                            .replace(/&#39;/g, "'");

    navigator.clipboard.writeText(decodedCode).then(() => {
      this.showToast('Code copied to clipboard', 'success');
    }).catch(err => {
      console.error('Copy failed:', err);
      this.showToast('Failed to copy code', 'error');
      
      // Fallback for browsers that don't support clipboard API
      const textarea = document.createElement('textarea');
      textarea.value = decodedCode;
      textarea.style.position = 'fixed';
      textarea.style.opacity = '0';
      document.body.appendChild(textarea);
      textarea.select();
      
      try {
        const successful = document.execCommand('copy');
        if (successful) {
          this.showToast('Code copied to clipboard (fallback method)', 'success');
        } else {
          this.showToast('Copy failed with fallback method', 'error');
        }
      } catch (err) {
        console.error('Fallback copy failed:', err);
        this.showToast('Failed to copy code', 'error');
      }
      
      document.body.removeChild(textarea);
    });
  },
  
  // Fetch OpenAPI specification based on selected spec
  async fetchOpenAPISpec() {
    try {
      const selectedSpecInfo = this.state.availableSpecs.find(spec => spec.id === this.state.selectedSpec);
      if (!selectedSpecInfo) {
        throw new Error('No selected specification available');
      }
      
      const response = await fetch(selectedSpecInfo.url, {
        method: 'GET',
        headers: {
          'Accept': 'application/json',
          'Cache-Control': 'no-cache'
        },
        credentials: 'include'
      });
      
      if (!response.ok) {
        throw new Error(`Failed to fetch OpenAPI spec: HTTP ${response.status} ${response.statusText}`);
      }

      const spec = await response.json();
      
      // Validate the spec has the required structure
      if (!spec.paths) {
        throw new Error('Invalid OpenAPI spec: missing "paths" property');
      }
      
      this.state.openApiSpec = spec;
      return spec;
      
    } catch (error) {
      console.error('Error fetching OpenAPI spec:', error);
      throw new Error(`Failed to load API specification: ${error.message}`);
    }
  },
  
  // UI State management
  setupUI() {
    this.showLoading(true);
  },
  
  showLoading(show) {
    const elements = {
      'loading-indicator': show ? 'block' : 'none',
      'routes-container': show ? 'none' : 'block',
      'error-message': 'none',
      'no-routes-message': 'none'
    };
    
    Object.entries(elements).forEach(([id, display]) => {
      const el = document.getElementById(id);
      if (el) el.style.display = display;
    });
  },
  
  showError(message) {
    const elements = {
      'loading-indicator': 'none',
      'routes-container': 'none',
      'error-message': 'block',
      'no-routes-message': 'none'
    };
    
    Object.entries(elements).forEach(([id, display]) => {
      const el = document.getElementById(id);
      if (el) el.style.display = display;
    });
    
    const errorEl = document.getElementById('error-message');
    if (errorEl) {
      const p = errorEl.querySelector('p');
      if (p) p.textContent = `❌ ${message}`;
    }
  },
  
  // Utility functions
  escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text || '';
    return div.innerHTML;
  },
  
  debounce(func, wait) {
    let timeout;
    return function(...args) {
      clearTimeout(timeout);
      timeout = setTimeout(() => func.apply(this, args), wait);
    };
  },
  
  showToast(message, type = 'info') {
    // Simple toast notification
    const existing = document.querySelector('.toast');
    if (existing) existing.remove();
    
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    toast.textContent = message;
    
    document.body.appendChild(toast);
    
    // Trigger animation
    setTimeout(() => toast.classList.add('show'), 50);
    
    setTimeout(() => {
      toast.classList.remove('show');
      setTimeout(() => toast.remove(), 300);
    }, 3000);
  },
  
  // Section expand/collapse functionality
  getSectionId(module) {
    return 'section-' + module.toLowerCase().replace(/[^a-z0-9]/g, '-');
  },
  
  getSectionCollapsedState(sectionId) {
    const stored = localStorage.getItem('makerapi-section-' + sectionId);
    return stored === 'collapsed';
  },
  
  setSectionCollapsedState(sectionId, collapsed) {
    localStorage.setItem('makerapi-section-' + sectionId, collapsed ? 'collapsed' : 'expanded');
  },
  
  toggleSection(sectionId) {
    const section = document.querySelector(`[data-section="${sectionId}"]`);
    if (!section) return;
    
    const content = section.querySelector('.api-section-content');
    const toggle = section.querySelector('.section-toggle');
    
    if (!content || !toggle) return;
    
    const isCurrentlyCollapsed = content.classList.contains('collapsed');
    
    if (isCurrentlyCollapsed) {
      // Expand
      content.classList.remove('collapsed');
      content.classList.add('expanded');
      toggle.classList.remove('collapsed');
      toggle.classList.add('expanded');
      toggle.textContent = '▼';
      this.setSectionCollapsedState(sectionId, false);
    } else {
      // Collapse
      content.classList.remove('expanded');
      content.classList.add('collapsed');
      toggle.classList.remove('expanded');
      toggle.classList.add('collapsed');
      toggle.textContent = '▶';
      this.setSectionCollapsedState(sectionId, true);
    }
  },
  
  // Update tag filter dropdown with available tags
  updateTagFilter() {
    const tagFilter = document.getElementById('tag-filter');
    if (!tagFilter) return;
    
    // Check if we've already populated the full tag list
    if (!this.state.allTags) {
      // First time - collect all unique tags from ALL routes (not filtered)
      const allTags = new Set();
      
      this.state.routes.forEach(route => {
        if (route.tags && Array.isArray(route.tags)) {
          route.tags.forEach(tag => {
            if (tag && tag.trim()) {
              allTags.add(this.formatModuleName(tag));
            }
          });
        } else {
          // Fallback to module name if no tags
          const moduleName = this.formatModuleName(route.module || 'Platform');
          allTags.add(moduleName);
        }
      });
      
      // Sort tags alphabetically and store for future use
      this.state.allTags = Array.from(allTags).sort();
    }
    
    // Preserve current selection
    const currentValue = tagFilter.value;
    
    // Clear and repopulate options with ALL tags
    tagFilter.innerHTML = '<option value="">All Tags</option>';
    
    this.state.allTags.forEach(tag => {
      const option = document.createElement('option');
      option.value = tag;
      option.textContent = tag;
      tagFilter.appendChild(option);
    });
    
    // Restore selection if it still exists
    if (currentValue && this.state.allTags.includes(currentValue)) {
      tagFilter.value = currentValue;
    }
  }
};

// Initialize when DOM is ready
if(document.readyState === "loading") {
  document.addEventListener('DOMContentLoaded', () => {
    MakerAPI.init();
    
    // Double-check event handlers after a short delay
    setTimeout(() => {
      MakerAPI.setupGlobalEventHandlers();
    }, 500);
  });
} else {
  MakerAPI.init();
  
  // Double-check event handlers after a short delay
  setTimeout(() => {
    MakerAPI.setupGlobalEventHandlers();
  }, 500);
}

// Make globally available
window.MakerAPI = MakerAPI;
)rawliteral";

#endif // MAKER_API_UTILS_JS_H
