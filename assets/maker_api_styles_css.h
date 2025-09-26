#ifndef MAKER_API_STYLES_CSS_H
#define MAKER_API_STYLES_CSS_H

#include <Arduino.h>

// API Documentation styles - designed to work alongside web_ui_styles.h
const char MAKER_API_STYLES_CSS[] PROGMEM = R"css(
/* Maker API Dashboard Styles */
/* Comprehensive interface with search, filtering, and enhanced route display */
/* Designed to complement web_platform_styles.css glass morphism theme */

/* Dashboard Layout */
.page-header {
  text-align: center;
  margin-bottom: 30px;
  color: #fff;
}

/* Top Row Container - Quadrant Layout */
.top-row-container {
  display: grid;
  gap: 20px;
  margin-bottom: 20px;
}

.top-row-container.has-spec {
  grid-template-columns: 1fr 1fr;
  grid-template-rows: auto auto;
}

.top-row-container.no-spec {
  grid-template-columns: 1fr;
  grid-template-rows: 1fr;
}

/* Quadrant Styling - Consistent across all quadrants */
.spec-quadrant,
#token-section,
.api-overview-section,
.quick-actions-section {
  padding: 15px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 10px;
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  transition: all 0.3s ease;
}

.spec-quadrant:hover,
#token-section:hover,
.api-overview-section:hover,
.quick-actions-section:hover {
  background: rgba(255, 255, 255, 0.12);
  border-color: rgba(255, 255, 255, 0.2);
}

/* Spec Quadrant Specific */
.spec-quadrant {
  display: flex;
  flex-direction: column;
  gap: 15px;
  min-height: 140px;
}

.spec-selector-container .form-group,
.server-info-container .form-group {
  margin: 0;
  height: 100%;
  display: flex;
  flex-direction: column;
}

/* Server Info Styling to match other quadrant content */
.server-details {
  font-size: 14px;
  background: rgba(255, 255, 255, 0.05);
  padding: 10px;
  border-radius: 6px;
  border: 1px solid rgba(255, 255, 255, 0.1);
}

.server-item {  
  display: flex;
  justify-content: space-between;
  margin-bottom: 4px;
  align-items: center;
}

.server-item:last-child {
  margin-bottom: 0;
}

.server-item .label {
  color: #fff
}

.server-item .value {
  font-family: monospace;
  font-size: 12px;
  background: rgba(0, 0, 0, 0.1);
  padding: 2px 6px;
  border-radius: 3px;
  border: 1px solid rgba(0, 0, 0, 0.1);
}

.server-status.online {
  color: #4CAF50;
  font-weight: 600;
  font-size: 12px;
  background: rgba(76, 175, 80, 0.1);
  padding: 2px 6px;
  border-radius: 3px;
  border: 1px solid rgba(76, 175, 80, 0.2);
}

/* Token Section Height Matching */
.has-spec #token-section {
  min-height: 140px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
}

/* Form Control Consistency in Quadrants */
.spec-quadrant .form-control,
#token-section .form-control {
  background: rgba(255, 255, 255, 0.1);
  border: 1px solid rgba(255, 255, 255, 0.2);
  color: #333;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 14px;
}

.spec-quadrant .form-control:focus,
#token-section .form-control:focus {
  background: rgba(255, 255, 255, 0.15);
  border-color: #2196F3;
  box-shadow: 0 0 0 2px rgba(33, 150, 243, 0.3);
  outline: none;
}

/* Small text consistency */
.spec-quadrant small,
#token-section small {
  color: #666;
  margin-top: 4px;
  display: block;
  font-size: 12px;
  line-height: 1.3;
}

.page-header h1 {
  font-size: 2.2em;
  margin-bottom: 10px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  text-shadow: 0 0 1px rgba(255, 255, 255, 0.5);
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  font-weight: 700;
}

.page-header p {
  color: rgba(255, 255, 255, 0.8);
  font-size: 1.1em;
}

/* API Statistics */
#api-stats {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
  gap: 15px;
  margin-top: 15px;
}

.api-stat {
  text-align: center;
  padding: 12px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 8px;
  border: 1px solid rgba(255, 255, 255, 0.1);
  transition: all 0.3s ease;
}

.api-stat:hover {
  background: rgba(255, 255, 255, 0.08);
  transform: translateY(-2px);
}

.stat-number {
  display: block;
  font-size: 2em;
  font-weight: bold;
  color: #4CCF50;
  margin-bottom: 5px;
}

.stat-label {
  font-size: 0.9em;
  color: rgba(255, 255, 255, 0.7);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

/* Action Buttons */
.action-buttons {
  display: flex;
  flex-direction: column;
  gap: 10px;
  margin-top: 15px;
}

.action-buttons .btn {
  padding: 10px 15px;
  font-size: 0.9em;
  width: 100%;
  min-height: 44px;
}

/* Routes Controls */
.routes-controls {
  display: flex;
  gap: 15px;
  margin-bottom: 20px;
  flex-wrap: wrap;
  align-items: center;
}

.search-container {
  flex: 2;
  min-width: 200px;
}

.filter-container {
  flex: 1;
  display: flex;
  gap: 10px;
  min-width: 300px;
}

.filter-container .form-control {
  min-width: 140px;
}

/* Loading States */
.loading-state {
  text-align: center;
  padding: 40px 20px;
  color: rgba(255, 255, 255, 0.8);
}

.loading-spinner {
  width: 40px;
  height: 40px;
  border: 4px solid rgba(255, 255, 255, 0.3);
  border-top: 4px solid #2196F3;
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin: 0 auto 15px;
}

.empty-state,
.error-state {
  text-align: center;
  padding: 40px 20px;
  color: rgba(255, 255, 255, 0.7);
  background: rgba(255, 255, 255, 0.05);
  border-radius: 10px;
  margin: 20px 0;
}

.empty-state p,
.error-state p {
  font-size: 1.1em;
  margin-bottom: 15px;
}

/* Enhanced Route Display */
.route-count {
  font-size: 0.8em;
  color: rgba(255, 255, 255, 0.6);
  font-weight: normal;
  margin-left: 8px;
}

.api-endpoints {
  margin-top: 15px;
}

.api-section {
  margin-bottom: 25px;
}

.api-section-title {
  color: #fff;
  font-size: 1.1em;
  font-weight: 600;
  margin-bottom: 12px;
  padding-bottom: 5px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.2);
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
  user-select: none;
  position: relative;
  transition: all 0.3s ease;
}

.api-section-title::before {
  content: "- ";
  font-size: 1em;
  opacity: 0.8;
}

.api-endpoint {
  margin-bottom: 10px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 8px;
  border: 1px solid rgba(255, 255, 255, 0.1);
  transition: all 0.3s ease;
  overflow: hidden;
}

.api-endpoint-header:hover {
  background: rgba(255, 255, 255, 0.08);
}

.api-endpoint:hover {
  border-color: rgba(255, 255, 255, 0.2);
  transform: translateX(2px);
}

.api-endpoint.expanded {
  background: rgba(255, 255, 255, 0.1);
  border-color: rgba(33, 150, 243, 0.5);
  transform: none;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
}

.api-endpoint-header {
  display: flex;
  align-items: center;
  padding: 12px;
  gap: 12px;
  flex-wrap: wrap;
  transition: all 0.3s ease;
  cursor: pointer;
}

.api-endpoint.expanded .api-endpoint-header {
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  background: rgba(255, 255, 255, 0.05);
}

.api-endpoint-content {
  max-height: 0;
  overflow: hidden;
  transition: max-height 0.4s ease, padding 0.4s ease;
  background: rgba(0, 0, 0, 0.2);
}

.api-endpoint.expanded .api-endpoint-content {
  max-height: 1000px;
  padding: 20px;
}

.api-method {
  font-family: 'Courier New', 'Monaco', 'Menlo', monospace;
  font-size: 0.8em;
  font-weight: bold;
  padding: 4px 8px;
  border-radius: 4px;
  min-width: 50px;
  text-align: center;
  color: #fff;
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.3);
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
}

.api-method.get {
  background: linear-gradient(135deg, rgba(76, 175, 80, 0.9), rgba(56, 142, 60, 0.9));
}

.api-method.post {
  background: linear-gradient(135deg, rgba(33, 150, 243, 0.9), rgba(25, 118, 210, 0.9));
}

.api-method.put {
  background: linear-gradient(135deg, rgba(255, 152, 0, 0.9), rgba(245, 124, 0, 0.9));
}

.api-method.patch {
  background: linear-gradient(135deg, rgba(156, 39, 176, 0.9), rgba(123, 31, 162, 0.9));
}

.api-method.delete {
  background: linear-gradient(135deg, rgba(244, 67, 54, 0.9), rgba(211, 47, 47, 0.9));
}

.api-path {
  font-family: 'Courier New', 'Monaco', 'Menlo', monospace;
  font-size: 0.9em;
  color: #fff;
  background: rgba(0, 0, 0, 0.2);
  padding: 6px 10px;
  border-radius: 4px;
  flex: 1;
  min-width: 200px;
  font-weight: 500;
  letter-spacing: 0.5px;
  border: 1px solid rgba(255, 255, 255, 0.1);
}

.api-description {
  color: rgba(255, 255, 255, 0.8);
  font-size: 0.9em;
  flex: 2;
  min-width: 150px;
  line-height: 1.4;
}

/* API Parameters and Response styling */
.api-details {
  margin-top: 8px;
  padding: 10px;
  background: rgba(0, 0, 0, 0.1);
  border-radius: 6px;
  border-left: 3px solid rgba(255, 255, 255, 0.3);
}

.api-params {
  font-size: 0.8em;
  color: rgba(255, 255, 255, 0.7);
  margin-top: 5px;
}

.api-params strong {
  color: rgba(255, 255, 255, 0.9);
}

.api-param-list {
  list-style: none;
  padding-left: 0;
  margin: 5px 0;
}

.api-param-list li {
  padding: 2px 0;
  font-family: 'Courier New', monospace;
  font-size: 0.85em;
}

.api-param-name {
  color: #4CAF50;
  font-weight: bold;
}

.api-param-type {
  color: #2196F3;
  font-style: italic;
}

.api-param-required {
  color: #FF5722;
  font-size: 0.7em;
  text-transform: uppercase;
  font-weight: bold;
}

/* Authentication indicator */
.api-auth-indicator {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  font-size: 0.7em;
  padding: 2px 6px;
  border-radius: 3px;
  margin-left: auto;
}

.api-auth-public {
  background: rgba(76, 175, 80, 0.3);
  color: #C8E6C9;
}

.api-auth-session {
  background: rgba(255, 152, 0, 0.3);
  color: #FFE0B2;
}

.api-auth-token {
  background: rgba(244, 67, 54, 0.3);
  color: #FFCDD2;
}

.api-auth-mixed {
  background: rgba(156, 39, 176, 0.3);
  color: #E1BEE7;
}

/* Section expand/collapse functionality */
.api-section-title {
  cursor: pointer;
  user-select: none;
  position: relative;
  transition: all 0.3s ease;
}

.api-section-title:hover {
  color: rgba(255, 255, 255, 0.9);
  background: rgba(255, 255, 255, 0.05);
  padding: 5px 10px;
  margin: -5px -10px;
  border-radius: 6px;
}

.section-toggle {
  display: inline-block;
  font-size: 0.8em;
  margin-right: 8px;
  transition: transform 0.3s ease;
  color: rgba(255, 255, 255, 0.7);
}

.section-toggle.collapsed {
  transform: rotate(-90deg);
}

.section-toggle.expanded {
  transform: rotate(0deg);
}

/* Section content animation */
.api-section-content {
  overflow: hidden;
  transition: max-height 0.3s ease, opacity 0.3s ease;
}

.api-section-content.expanded {
  max-height: none;
  opacity: 1;
}

.api-section-content.collapsed {
  max-height: 0;
  opacity: 0;
}

/* API status badges */
.api-status-badge {
  display: inline-block;
  font-size: 0.7em;
  padding: 2px 6px;
  border-radius: 10px;
  font-weight: bold;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.api-status-stable {
  background: rgba(76, 175, 80, 0.3);
  color: #C8E6C9;
}

.api-status-beta {
  background: rgba(255, 152, 0, 0.3);
  color: #FFE0B2;
}

.api-status-deprecated {
  background: rgba(244, 67, 54, 0.3);
  color: #FFCDD2;
}

/* Responsive design for quadrants */
@media (max-width: 768px) {
  .top-row-container.has-spec {
    grid-template-columns: 1fr !important;
    gap: 15px;
  }
  
  .spec-quadrant,
  #token-section {
    min-height: auto;
  }
  
  .server-item {
    flex-direction: column;
    align-items: flex-start;
    gap: 2px;
  }
  
  .server-item .value {
    font-size: 11px;
    word-break: break-all;
  }
}

/* Responsive design for API documentation */
@media (max-width: 600px) {
  .api-endpoint {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
  
  .api-method {
    align-self: flex-start;
    min-width: 60px;
  }
  
  .api-path {
    width: 100%;
    min-width: unset;
    word-break: break-all;
  }
  
  .api-description {
    width: 100%;
    min-width: unset;
  }
  
  .api-auth-indicator {
    margin-left: 0;
    margin-top: 5px;
  }
  
  .api-section-title {
    font-size: 1em;
  }
  
  .api-details {
    margin-top: 10px;
    padding: 8px;
  }
}

/* Syntax highlighting for code examples */
.api-code-example {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 6px;
  padding: 12px;
  margin: 10px 0;
  font-family: 'Courier New', monospace;
  font-size: 0.85em;
  color: #E8E8E8;
  overflow-x: auto;
  white-space: pre;
}

.api-code-example .keyword {
  color: #569CD6;
}

.api-code-example .string {
  color: #CE9178;
}

.api-code-example .number {
  color: #B5CEA8;
}

.api-code-example .comment {
  color: #6A9955;
  font-style: italic;
}

/* API section separators */
.api-section:not(:last-child)::after {
  content: "";
  display: block;
  height: 1px;
  background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.2), transparent);
  margin: 20px 0;
}

/* Animations */
@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

/* Token management and API testing styles */
.token-section {
  transition: all 0.3s ease;
}

.token-section.token-valid {
  border: 2px solid rgba(76, 175, 80, 0.5);
  background: rgba(76, 175, 80, 0.05);
}

.token-controls {
  margin-top: 15px;
}

.token-controls .form-group {
  margin-bottom: 15px;
}

.token-buttons {
  display: flex;
  gap: 10px;
  margin-top: 10px;
}

.token-buttons .btn {
  flex: 1;
}

.token-status {
  margin-top: 10px;
  padding: 8px;
  border-radius: 4px;
  font-size: 0.9em;
}

.token-status .success {
  color: #4CAF50;
  background: rgba(76, 175, 80, 0.1);
  border: 1px solid rgba(76, 175, 80, 0.3);
}

.token-status .error {
  color: #F44336;
  background: rgba(244, 67, 54, 0.1);
  border: 1px solid rgba(244, 67, 54, 0.3);
}

.token-status .info {
  color: #2196F3;
  background: rgba(33, 150, 243, 0.1);
  border: 1px solid rgba(33, 150, 243, 0.3);
}

.token-help {
  margin-top: 20px;
  padding: 15px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 6px;
  border-left: 3px solid rgba(33, 150, 243, 0.5);
}

.token-help p {
  color: rgba(255, 255, 255, 0.8);
  margin-bottom: 10px;
  font-size: 0.9em;
}

.token-help ol {
  color: rgba(255, 255, 255, 0.7);
  font-size: 0.9em;
  padding-left: 20px;
}

.token-help a {
  color: #2196F3;
  text-decoration: none;
}

.token-help a:hover {
  text-decoration: underline;
}

/* Endpoint expand indicator */
.endpoint-expand-indicator {
  margin-left: auto;
  font-size: 0.8em;
  color: rgba(255, 255, 255, 0.5);
  transition: all 0.3s ease;
  pointer-events: none;
}

.api-endpoint.expanded .endpoint-expand-indicator {
  transform: rotate(180deg);
  color: rgba(255, 255, 255, 0.8);
}

/* Endpoint content tabs */
.endpoint-tabs {
  margin-bottom: 20px;
}

.endpoint-tab-buttons {
  display: flex;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  margin-bottom: 20px;
  gap: 2px;
}

.endpoint-tab-button {
  background: none;
  border: none;
  padding: 12px 20px;
  color: rgba(255, 255, 255, 0.7);
  cursor: pointer;
  transition: all 0.3s ease;
  border-bottom: 2px solid transparent;
  font-size: 0.9em;
  font-weight: 500;
  flex: 1;
  text-align: center;
}

.endpoint-tab-button:hover {
  color: rgba(255, 255, 255, 0.9);
  background: rgba(255, 255, 255, 0.05);
}

.endpoint-tab-button.active {
  color: #fff;
  border-bottom-color: #2196F3;
  background: rgba(33, 150, 243, 0.1);
}

.endpoint-tab-content {
  min-height: 200px;
}

.endpoint-tab-panel {
  display: none;
  animation: fadeIn 0.3s ease;
}

.endpoint-tab-panel.active {
  display: block !important;
}



/* Parameter Section - shared styling */
.parameter-input {
  margin-bottom: 15px;
}

.parameter-input label {
  display: block;
  color: rgba(255, 255, 255, 0.9);
  font-size: 0.9em;
  margin-bottom: 5px;
  font-weight: 500;
}

.param-description {
  display: block;
  color: rgba(255, 255, 255, 0.6);
  font-size: 0.8em;
  font-weight: normal;
  margin-top: 2px;
  font-style: italic;
}

/* Parameter input field styling */
.parameter-input input,
.parameter-input select {
  background: rgba(255, 255, 255, 0.1) !important;
  border: 1px solid rgba(255, 255, 255, 0.2) !important;
  color: #fff !important;
}

.parameter-input input::placeholder,
.parameter-input select option:first-child:disabled {
  color: rgba(255, 255, 255, 0.4) !important;
  font-style: italic;
  font-weight: 300;
}

.parameter-input input:focus,
.parameter-input select:focus {
  background: rgba(255, 255, 255, 0.15) !important;
  border-color: #2196F3 !important;
  box-shadow: 0 0 0 2px rgba(33, 150, 243, 0.3) !important;
}

/* Response styling - shared */
.response-status {
  margin-bottom: 10px;
}

.response-body {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 6px;
  padding: 15px;
  max-height: 300px;
  overflow-y: auto;
}

.response-body pre {
  margin: 0;
  color: #E8E8E8;
  font-family: 'Courier New', monospace;
  font-size: 0.85em;
  line-height: 1.4;
  white-space: pre-wrap;
  word-break: break-word;
}

/* Button States */
.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
  transform: none !important;
}



/* Code Section - consolidated styling */
.code-section {
  margin-bottom: 20px;
}

.code-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
}

.code-header span {
  color: rgba(255, 255, 255, 0.9);
  font-weight: 500;
}

.copy-code-btn {
  font-size: 0.8em;
  padding: 6px 12px;
}

.code-block {
  background: rgba(0, 0, 0, 0.4);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 6px;
  padding: 15px;
  overflow-x: auto;
  font-family: 'Courier New', monospace;
  font-size: 0.85em;
  line-height: 1.5;
  color: #E8E8E8;
  white-space: pre-wrap;
  word-break: break-word;
  max-height: 250px;
  overflow-y: auto;
}

.code-block code {
  color: inherit;
  background: none;
  padding: 0;
  font-size: inherit;
  font-family: inherit;
}

/* Route Details */
.route-details {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 8px;
  padding: 20px;
  margin-bottom: 20px;
}

.detail-item {
  display: flex;
  margin-bottom: 12px;
  color: rgba(255, 255, 255, 0.8);
  font-size: 0.9em;
}

.detail-item strong {
  color: rgba(255, 255, 255, 0.9);
  min-width: 120px;
  font-weight: 500;
}

.detail-item:last-child {
  margin-bottom: 0;
}

/* Endpoint details styling (for expanded routes) */
.endpoint-tab-panel .route-details {
  margin-bottom: 10px;
  padding: 15px;
}



/* Endpoint content styling */
.endpoint-try-section {
  margin-bottom: 20px;
}

.endpoint-try-section .api-test-info {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 20px;
  padding: 15px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 8px;
  flex-wrap: wrap;
}

.endpoint-parameter-section {
  margin-bottom: 20px;
}

.endpoint-parameter-section h4 {
  color: #fff;
  margin-bottom: 15px;
  font-size: 1.1em;
}

/* Request Body Section */
.endpoint-body-section {
  margin-bottom: 20px;
}

.endpoint-body-section h4 {
  color: #fff;
  margin-bottom: 15px;
  font-size: 1.1em;
}

.request-body-controls {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.request-body-controls label {
  color: rgba(255, 255, 255, 0.9);
  font-size: 0.9em;
  font-weight: 500;
}

.code-input {
  font-family: 'Courier New', 'Monaco', 'Menlo', monospace !important;
  font-size: 0.85em !important;
  line-height: 1.4;
  background: rgba(0, 0, 0, 0.4) !important;
  border: 1px solid rgba(255, 255, 255, 0.2) !important;
  color: #E8E8E8 !important;
  padding: 12px !important;
  border-radius: 6px;
  resize: vertical;
  min-height: 120px;
  transition: all 0.3s ease;
}

.code-input:focus {
  background: rgba(0, 0, 0, 0.5) !important;
  border-color: #2196F3 !important;
  box-shadow: 0 0 0 2px rgba(33, 150, 243, 0.3) !important;
}

.code-input::placeholder {
  color: rgba(255, 255, 255, 0.5);
  font-family: 'Courier New', 'Monaco', 'Menlo', monospace;
}

.endpoint-test-results {
  margin-top: 20px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
  padding-top: 20px;
  display: none;
}

.endpoint-test-results.show {
  display: block;
}

.endpoint-test-actions {
  display: flex;
  gap: 10px;
  margin-bottom: 20px;
  flex-wrap: wrap;
}

.endpoint-test-actions .btn {
  flex: 1;
  min-width: 120px;
}

/* Animation for tab transitions */
@keyframes fadeIn {
  from {
    opacity: 0;
    transform: translateY(10px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* Search indicator for real-time feedback */
.search-container {
  position: relative;
}

.search-container.searching::after {
  content: "🔍";
  position: absolute;
  right: 10px;
  top: 50%;
  transform: translateY(-50%);
  animation: pulse 1.5s infinite;
}

/* Enhanced button states */
.btn.loading {
  position: relative;
  color: transparent;
}

.btn.loading::after {
  content: "";
  position: absolute;
  width: 16px;
  height: 16px;
  top: 50%;
  left: 50%;
  margin-left: -8px;
  margin-top: -8px;
  border: 2px solid transparent;
  border-top-color: #ffffff;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

/* Pulse animation for search indicator */
@keyframes pulse {
  0%, 100% {
    opacity: 1;
    transform: translateY(-50%) scale(1);
  }
  50% {
    opacity: 0.5;
    transform: translateY(-50%) scale(1.1);
  }
}

/* Enhanced error states */
.error-state {
  text-align: center;
  padding: 40px 20px;
  color: rgba(255, 255, 255, 0.8);
  background: rgba(244, 67, 54, 0.1);
  border: 1px solid rgba(244, 67, 54, 0.3);
  border-radius: 10px;
  margin: 20px 0;
}

.error-state p:first-child {
  font-size: 1.2em;
  font-weight: 600;
  margin-bottom: 10px;
}

/* Improved accessibility */
.btn:focus,
.form-control:focus,
.endpoint-tab-button:focus {
  outline: 2px solid #2196F3;
  outline-offset: 2px;
}

/* Better visual hierarchy */
.page-header {
  margin-bottom: 40px;
}

.routes-controls {
  margin-bottom: 25px;
  padding: 20px;
  background: rgba(255, 255, 255, 0.03);
  border-radius: 10px;
  border: 1px solid rgba(255, 255, 255, 0.1);
}

/* Mobile responsive enhancements for dashboard */
@media (max-width: 768px) {
  .page-header h1 {
    font-size: 1.8em;
  }
  
  .routes-controls {
    flex-direction: column;
    gap: 10px;
    padding: 15px;
  }
  
  .search-container,
  .filter-container {
    width: 100%;
    min-width: unset;
  }
  
  .filter-container {
    flex-direction: column;
    gap: 8px;
  }
  
  #api-stats {
    grid-template-columns: repeat(2, 1fr);
    gap: 10px;
  }
  
  .action-buttons .btn {
    font-size: 0.8em;
    padding: 8px 12px;
  }
}

@media (max-width: 480px) {
  #api-stats {
    grid-template-columns: 1fr;
  }
  
  .stat-number {
    font-size: 1.5em;
  }
  
  .loading-spinner {
    width: 30px;
    height: 30px;
  }
  
  .token-buttons {
    flex-direction: column;
  }
  
  .modal-content {
    width: 95%;
    max-height: 95vh;
  }
  
  .modal-body {
    padding: 15px;
  }
  
  .api-test-info {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
  
  .test-actions {
    flex-direction: column;
  }
  
  .endpoint-tab-buttons {
    flex-direction: column;
    gap: 4px;
  }
  
  .endpoint-tab-button {
    text-align: left;
    padding: 10px 15px;
  }
  
  .endpoint-test-actions {
    flex-direction: column;
  }
  
  .api-endpoint-header {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
  
  .api-path {
    width: 100%;
    min-width: unset;
    word-break: break-all;
  }
  
  .api-description {
    width: 100%;
    min-width: unset;
  }
  
  .endpoint-expand-indicator {
    margin-left: auto;
    align-self: flex-end;
  }
}
/* Wider container for better path display */
.api-endpoints .api-endpoint {
  /* Allow endpoint cards to be slightly wider when needed */
  max-width: none;
}

/* Container adjustments for better path visibility */
@media (min-width: 1200px) {
  .container {
    max-width: 1140px;
  }
  
  .api-path {
    min-width: 250px; /* Increased minimum width on larger screens */
  }
}

@media (min-width: 1400px) {
  .container {
    max-width: 1320px;
  }
  
  .api-path {
    min-width: 300px; /* Even more space for very wide screens */
  }
}
)css";

#endif // MAKER_API_STYLES_H