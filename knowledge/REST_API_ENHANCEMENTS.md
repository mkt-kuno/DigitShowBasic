# REST API Enhancements

## Overview

This document describes recent enhancements to the DigitShowDST REST API, including the OpenAPI specification endpoint and automatic configuration file generation.

## New Features

### 1. OpenAPI Specification Endpoint

The REST API now serves its OpenAPI 3.0 specification directly via HTTP.

#### Endpoint

```
GET /api/openapi
```

#### Response

- **Content-Type**: `application/x-yaml; charset=utf-8`
- **Body**: OpenAPI 3.0 specification in YAML format

#### Example Usage

```bash
# Fetch the OpenAPI specification
curl http://localhost:8080/api/openapi

# Save to a file
curl http://localhost:8080/api/openapi -o openapi.yaml
```

#### Implementation Details

- The specification is embedded at build time as a C++ `constexpr std::string_view` in `src/openapi_spec.hpp`
- No disk I/O required to serve the specification
- Content is synchronized with `docs/api/openapi.yaml`
- The specification documents all available API endpoints including itself

### 2. Automatic Configuration File Generation

The API server now automatically creates `api_config.json` with default values if it doesn't exist.

#### Behavior

When `ApiServer::load_config("api_config.json")` is called:

1. **File exists**: Loads configuration from the file
2. **File missing**:
   - Creates `api_config.json` in the same directory as the executable
   - Populates with default values from the `ApiConfig` struct
   - Logs the creation
   - Returns default configuration

#### Default Configuration

```json
{
  "$schema": "schemas/api_config.schema.json",
  "enabled": false,
  "host": "127.0.0.1",
  "port": 8080,
  "update_interval_ms": 100,
  "cors_enabled": true,
  "max_connections": 10
}
```

#### Benefits

- **First-run experience**: No manual configuration file creation needed
- **Self-documenting**: Generated file includes JSON schema reference
- **Fail-safe**: Application continues with defaults even if file creation fails
- **Standard location**: Configuration file is created next to the executable

#### Implementation Details

- Uses `std::filesystem::exists()` to check for file existence
- Uses `std::ofstream` for file writing
- Pretty-prints JSON with 2-space indentation
- All file operations use standard C++ library (no MFC dependencies)
- Comprehensive logging via spdlog

## API Endpoints Summary

After these enhancements, the REST API provides:

| Endpoint | Method | Description | Content-Type |
|----------|--------|-------------|--------------|
| `/api/health` | GET | Health check | `application/json` |
| `/api/openapi` | GET | OpenAPI specification | `application/x-yaml` |
| `/api/sensor-data` | GET | Current sensor data snapshot | `application/json` |
| `/api/sensor-data/stream` | GET | Real-time SSE data stream | `text/event-stream` |

## Configuration Schema

The `api_config.json` file follows the schema defined in `schemas/api_config.schema.json`:

```json
{
  "enabled": boolean,           // Enable/disable the API server
  "host": string,               // Bind address (e.g., "127.0.0.1")
  "port": integer,              // Port number (1024-65535)
  "update_interval_ms": integer,// SSE update interval (10-10000 ms)
  "cors_enabled": boolean,      // Enable CORS headers
  "max_connections": integer    // Max concurrent connections (1-100)
}
```

## Usage Examples

### Enabling the API Server

1. Set `"enabled": true` in `api_config.json`
2. Optionally adjust host, port, and other settings
3. Restart the application

### Accessing the OpenAPI Specification

```bash
# View in terminal
curl http://localhost:8080/api/openapi

# Import into Swagger Editor
curl http://localhost:8080/api/openapi | swagger-editor

# Generate client code
curl http://localhost:8080/api/openapi -o api.yaml
openapi-generator generate -i api.yaml -g python -o ./client
```

### Integrating with API Tools

The OpenAPI specification can be used with various tools:

- **Swagger UI**: Interactive API documentation
- **Postman**: Import and test endpoints
- **OpenAPI Generator**: Generate client libraries
- **VS Code REST Client**: Use with the REST Client extension

## Technical Notes

### Build-Time Embedding

The OpenAPI specification is embedded at compile time:

```cpp
namespace api {
constexpr std::string_view kOpenApiYaml = R"(
openapi: 3.0.3
info:
  title: DigitShowDST Sensor Data API
  ...
)";
}
```

**Advantages**:
- Zero runtime overhead
- No file system dependencies
- Cannot be accidentally deleted or corrupted
- Always synchronized with build

**Updating the Spec**:
1. Edit `docs/api/openapi.yaml`
2. Update `src/openapi_spec.hpp` with the same content
3. Rebuild the application

### File System Operations

All file operations use standard C++ libraries:

```cpp
#include <filesystem>
#include <fstream>

// Check existence
std::filesystem::exists(path)

// Create file
std::ofstream out_file(path)
```

No MFC or Windows-specific APIs are used, maintaining cross-platform compatibility and modern C++ practices.

## Error Handling

### Configuration Loading

- **File not found**: Creates default, logs warning
- **Parse error**: Uses defaults, logs error
- **Write failure**: Uses defaults, logs error

All error conditions are handled gracefully without crashing the application.

### OpenAPI Endpoint

- **Exception during serving**: Returns HTTP 500 with error JSON
- **Invalid request**: Standard HTTP error handling via cpp-httplib

## Logging

The implementation provides detailed logging:

```
[info] API config file not found: api_config.json. Creating with default values.
[info] Created default API config file: api_config.json
[info] Loaded API config from: api_config.json
[debug] Served OpenAPI specification
```

Log levels:
- `info`: Normal operations (config load, file creation)
- `warn`: Non-critical issues (file not found)
- `error`: Failures (parse errors, write failures)
- `debug`: Detailed operation traces (spec served)

## Future Enhancements

Potential improvements for future versions:

1. **Content Negotiation**: Serve JSON format via Accept header
2. **Versioned Specs**: Support multiple API versions
3. **Dynamic Generation**: Generate spec from C++ code annotations
4. **Hot Reload**: Reload configuration without restart
5. **Web UI**: Embedded Swagger UI for interactive documentation

## References

- OpenAPI 3.0 Specification: https://spec.openapis.org/oas/v3.0.3
- cpp-httplib Documentation: https://github.com/yhirose/cpp-httplib
- API Schema: `schemas/api_config.schema.json`
- Original Spec: `docs/api/openapi.yaml`
- Implementation: `src/ApiServer.cpp`, `src/openapi_spec.hpp`
