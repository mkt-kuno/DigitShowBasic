# Testing REST API Enhancements

This document provides step-by-step instructions for testing the new REST API features.

## Prerequisites

- Build the application with the latest changes
- Ensure the executable is in your working directory

## Test 1: Auto-Generated Configuration File

### Expected Behavior
When the application starts and `api_config.json` doesn't exist, it should be automatically created.

### Steps

1. **Remove existing config** (if present):
   ```cmd
   del api_config.json
   ```

2. **Start the application**:
   ```cmd
   DigitShowDST.exe
   ```

3. **Check the log output**:
   Look for these log messages:
   ```
   [warn] API config file not found: api_config.json. Creating with default values.
   [info] Created default API config file: api_config.json
   ```

4. **Verify the file was created**:
   ```cmd
   dir api_config.json
   ```

5. **Inspect the file contents**:
   ```cmd
   type api_config.json
   ```

   Expected output:
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

### Success Criteria
- ✅ File is created automatically
- ✅ File contains proper JSON structure
- ✅ All default values are present
- ✅ File location is next to the executable
- ✅ Application logs the creation

## Test 2: OpenAPI Endpoint

### Prerequisites
Enable the API server:

1. Edit `api_config.json`:
   ```json
   {
     "enabled": true,
     "host": "127.0.0.1",
     "port": 8080,
     ...
   }
   ```

2. Start or restart the application

### Steps

1. **Verify API server started**:
   Check logs for:
   ```
   [info] API server started successfully at http://127.0.0.1:8080
   ```

2. **Test the OpenAPI endpoint** using curl:
   ```cmd
   curl http://localhost:8080/api/openapi
   ```

   Or using PowerShell:
   ```powershell
   Invoke-WebRequest -Uri "http://localhost:8080/api/openapi" | Select-Object -ExpandProperty Content
   ```

3. **Save the specification**:
   ```cmd
   curl http://localhost:8080/api/openapi -o openapi-from-api.yaml
   ```

4. **Verify the content**:
   - Check the Content-Type header: `application/x-yaml; charset=utf-8`
   - Verify it starts with `openapi: 3.0.3`
   - Verify it contains 4 paths: `/api/health`, `/api/openapi`, `/api/sensor-data`, `/api/sensor-data/stream`

### Alternative Test Methods

#### Using a Web Browser
1. Navigate to: `http://localhost:8080/api/openapi`
2. Browser should prompt to download or display the YAML file

#### Using Postman
1. Create a new GET request to `http://localhost:8080/api/openapi`
2. Send the request
3. Verify the response contains valid YAML

#### Using Swagger Editor
1. Open https://editor.swagger.io/
2. Click "File" → "Import URL"
3. Enter: `http://localhost:8080/api/openapi`
4. Swagger Editor should parse and display the API documentation

### Success Criteria
- ✅ Endpoint responds with HTTP 200
- ✅ Content-Type is `application/x-yaml`
- ✅ Response contains valid OpenAPI 3.0 YAML
- ✅ All 4 endpoints are documented
- ✅ Specification includes schemas for all data models
- ✅ No errors in application logs

## Test 3: Integration Test

### Steps

1. **Start fresh**:
   ```cmd
   del api_config.json
   DigitShowDST.exe
   ```
   - Verify `api_config.json` is created with `"enabled": false`

2. **Enable API and restart**:
   - Edit `api_config.json`, set `"enabled": true`
   - Restart application
   - Verify API server starts

3. **Test all endpoints**:
   ```cmd
   REM Health check
   curl http://localhost:8080/api/health

   REM OpenAPI spec
   curl http://localhost:8080/api/openapi

   REM Current sensor data
   curl http://localhost:8080/api/sensor-data

   REM Note: SSE stream requires special handling
   REM curl http://localhost:8080/api/sensor-data/stream
   ```

4. **Verify responses**:
   - Health: Returns JSON with `"status": "ok"`
   - OpenAPI: Returns YAML specification
   - Sensor data: Returns JSON with timestamp, physical_input, physical_output

### Success Criteria
- ✅ All endpoints respond correctly
- ✅ No errors in application logs
- ✅ Configuration persists across restarts
- ✅ Auto-generation works on first run

## Test 4: Error Handling

### Test Missing Config Directory

1. **Create a subdirectory and run from there**:
   ```cmd
   mkdir test
   cd test
   ..\DigitShowDST.exe
   ```

2. **Verify**:
   - Config is created in the test directory
   - Application works normally

### Test Corrupted Config

1. **Create invalid JSON**:
   ```cmd
   echo invalid json > api_config.json
   ```

2. **Start application**:
   ```cmd
   DigitShowDST.exe
   ```

3. **Verify**:
   - Application logs error: `[error] Error loading API config from api_config.json: ...`
   - Application continues with defaults: `[info] Using defaults`
   - Application doesn't crash

### Test Read-Only Config

1. **Make config read-only**:
   ```cmd
   attrib +R api_config.json
   ```

2. **Delete and restart**:
   ```cmd
   del api_config.json /F
   DigitShowDST.exe
   ```

3. **Verify**:
   - Application logs failure: `[error] Failed to create API config file: ...`
   - Application continues with defaults
   - Application doesn't crash

### Success Criteria
- ✅ Graceful handling of missing directories
- ✅ Graceful handling of corrupted files
- ✅ Graceful handling of write failures
- ✅ Application never crashes due to config issues
- ✅ Clear error messages in logs

## Troubleshooting

### API Server Not Starting

**Symptom**: Log shows `[info] API server is disabled in configuration`

**Solution**: Edit `api_config.json` and set `"enabled": true`

### Port Already in Use

**Symptom**: Log shows `[error] Failed to start API server on 127.0.0.1:8080`

**Solution**:
1. Check if another application is using port 8080:
   ```cmd
   netstat -ano | findstr :8080
   ```
2. Either stop the other application or change the port in `api_config.json`

### Config File Not Created

**Symptom**: Config file doesn't appear after starting application

**Possible Causes**:
1. No write permission in the directory
2. Running from a protected location (e.g., Program Files)

**Solution**:
- Run from a user-writable location
- Check Windows Event Log for access denied errors
- Run application as administrator (not recommended for normal use)

### OpenAPI Endpoint Returns 404

**Symptom**: `curl http://localhost:8080/api/openapi` returns 404

**Possible Causes**:
1. API server not enabled
2. Wrong URL

**Solution**:
1. Verify API server is running: check logs for "API server started successfully"
2. Verify URL is correct: `http://localhost:8080/api/openapi` (no trailing slash)
3. Try health check first: `http://localhost:8080/api/health`

## Verification Checklist

Use this checklist to verify all features are working:

- [ ] Application creates `api_config.json` on first run
- [ ] Config file contains all required fields
- [ ] Config file has proper JSON formatting
- [ ] API server starts when enabled
- [ ] `/api/health` endpoint works
- [ ] `/api/openapi` endpoint returns valid YAML
- [ ] `/api/sensor-data` endpoint returns valid JSON
- [ ] OpenAPI spec matches `docs/api/openapi.yaml`
- [ ] Application logs are informative
- [ ] Error handling works for corrupted config
- [ ] Error handling works for read-only filesystem

## Reporting Issues

If you find any issues during testing, please report:

1. **Steps to reproduce** the issue
2. **Expected behavior** vs. **actual behavior**
3. **Log output** from the application
4. **System information**: Windows version, build number
5. **Screenshots** if applicable

Include the contents of `api_config.json` if relevant.
