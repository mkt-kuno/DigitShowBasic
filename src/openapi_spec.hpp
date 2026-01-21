/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Takuto ISHII
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

/**
 * @file openapi_spec.hpp
 * @brief Embedded OpenAPI specification
 *
 * This file contains the OpenAPI specification embedded as a string constant.
 * It is generated from docs/api/openapi.yaml.
 */

#include <string_view>

namespace api
{

constexpr std::string_view kOpenApiYaml = R"(openapi: 3.0.3
info:
  title: DigitShowDST Sensor Data API
  description: |
    REST API for accessing real-time sensor data from the DigitShowDST geotechnical testing apparatus.
    Provides server-sent events (SSE) for streaming sensor measurements including:
    - Physical forces and displacements
    - Stress and strain measurements
    - Control outputs (motor speed, EP values)
  version: 1.0.0
  contact:
    name: DigitShowDST Project
    url: https://github.com/takker99/DigitShowDST

servers:
  - url: http://localhost:8080
    description: Local development server

paths:
  /api/health:
    get:
      summary: Health check endpoint
      description: Returns the health status of the API server
      operationId: getHealth
      responses:
        '200':
          description: Server is healthy
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
                    example: "ok"
                  timestamp:
                    type: integer
                    format: int64
                    description: Unix timestamp in milliseconds
                    example: 1704153600000

  /api/openapi:
    get:
      summary: Get OpenAPI specification (JSON)
      description: Returns the OpenAPI 3.0 specification in JSON format by default
      operationId: getOpenApiJson
      responses:
        '200':
          description: OpenAPI specification in JSON format
          content:
            application/json:
              schema:
                type: object
                description: OpenAPI 3.0 specification as JSON object

  /api/openapi.json:
    get:
      summary: Get OpenAPI specification (JSON)
      description: Returns the OpenAPI 3.0 specification in JSON format
      operationId: getOpenApiJsonExplicit
      responses:
        '200':
          description: OpenAPI specification in JSON format
          content:
            application/json:
              schema:
                type: object
                description: OpenAPI 3.0 specification as JSON object

  /api/openapi.yml:
    get:
      summary: Get OpenAPI specification (YAML)
      description: Returns the OpenAPI 3.0 specification in YAML format
      operationId: getOpenApiYaml
      responses:
        '200':
          description: OpenAPI specification in YAML format
          content:
            application/x-yaml:
              schema:
                type: string
                description: OpenAPI 3.0 specification in YAML format

  /api/sensor-data:
    get:
      summary: Get current sensor data (snapshot)
      description: Returns the latest sensor data snapshot as JSON
      operationId: getSensorData
      responses:
        '200':
          description: Current sensor data
          content:
            application/json:
              schema:
                $ref: '#/components/schemas/SensorData'

  /api/sensor-data/stream:
    get:
      summary: Stream sensor data via Server-Sent Events
      description: |
        Opens a server-sent events (SSE) connection that streams real-time sensor data.
        Data is sent at the configured update interval (default: 100ms).
      operationId: streamSensorData
      responses:
        '200':
          description: SSE stream of sensor data
          content:
            text/event-stream:
              schema:
                type: string
                description: |
                  Server-sent event stream. Each event contains:
                  - event: "data"
                  - data: JSON-encoded SensorData object
              example: |
                event: data
                data: {"timestamp": 1704153600000, "physical_input": {...}, "physical_output": {...}}

                event: data
                data: {"timestamp": 1704153600100, "physical_input": {...}, "physical_output": {...}}

components:
  schemas:
    SensorData:
      type: object
      description: Complete sensor data snapshot
      required:
        - timestamp
        - physical_input
        - physical_output
      properties:
        timestamp:
          type: integer
          format: int64
          description: Unix timestamp in milliseconds when the data was captured
          example: 1704153600000
        physical_input:
          $ref: '#/components/schemas/PhysicalInput'
        physical_output:
          $ref: '#/components/schemas/PhysicalOutput'

    PhysicalInput:
      type: object
      description: Physical measurements from sensors (A/D inputs)
      required:
        - specimen
        - shear_force_N
        - vertical_force_N
        - shear_displacement_mm
        - front_vertical_disp_mm
        - rear_vertical_disp_mm
        - front_friction_force_N
        - rear_friction_force_N
        - shear_stress_kpa
        - vertical_stress_kpa
        - normal_displacement_mm
        - tilt_mm
      properties:
        specimen:
          $ref: '#/components/schemas/SpecimenSnapshot'
        shear_force_N:
          type: number
          format: double
          description: Shear force in Newtons
          example: 125.3
        vertical_force_N:
          type: number
          format: double
          description: Vertical force in Newtons
          example: 450.8
        shear_displacement_mm:
          type: number
          format: double
          description: Shear displacement in millimeters
          example: 2.45
        front_vertical_disp_mm:
          type: number
          format: double
          description: Front vertical displacement in millimeters
          example: 0.82
        rear_vertical_disp_mm:
          type: number
          format: double
          description: Rear vertical displacement in millimeters
          example: 0.78
        front_friction_force_N:
          type: number
          format: double
          description: Front friction force in Newtons
          example: 5.2
        rear_friction_force_N:
          type: number
          format: double
          description: Rear friction force in Newtons
          example: 5.1
        shear_stress_kpa:
          type: number
          format: double
          description: Calculated shear stress τ in kPa
          example: 10.5
        vertical_stress_kpa:
          type: number
          format: double
          description: Calculated vertical stress σ in kPa
          example: 31.3
        normal_displacement_mm:
          type: number
          format: double
          description: Average normal displacement in millimeters
          example: 0.80
        tilt_mm:
          type: number
          format: double
          description: Half of the difference between front and rear vertical displacement (specimen tilt)
          example: 0.02

    PhysicalOutput:
      type: object
      description: Control outputs (D/A outputs)
      required:
        - front_ep_kpa
        - rear_ep_kpa
        - motor_rpm
      properties:
        front_ep_kpa:
          type: number
          format: double
          description: Front effective pressure in kPa
          example: 100.0
        rear_ep_kpa:
          type: number
          format: double
          description: Rear effective pressure in kPa
          example: 100.0
        motor_rpm:
          type: number
          format: double
          description: Motor rotation speed in RPM (positive = forward, negative = reverse, 0 = stopped)
          example: 30.0

    SpecimenSnapshot:
      type: object
      description: Specimen geometry and weight information
      required:
        - height_mm
        - area_mm2
        - weight_g
        - box_weight_g
      properties:
        height_mm:
          type: number
          format: double
          description: Current specimen height in millimeters
          example: 120.0
        area_mm2:
          type: number
          format: double
          description: Specimen cross-sectional area in square millimeters
          example: 14400.0
        volume_mm3:
          type: number
          format: double
          description: Calculated specimen volume in cubic millimeters
          example: 1728000.0
        weight_g:
          type: number
          format: double
          description: Specimen weight in grams
          example: 3500.0
        box_weight_g:
          type: number
          format: double
          description: Shear box weight in grams
          example: 10000.0
)";

} // namespace api
