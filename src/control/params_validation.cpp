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

/**
 * @file control/params_validation.cpp
 * @brief Validation of ControlParams conversion functions
 *
 * This file runs runtime tests to ensure conversion functions work correctly.
 */

#include "../StdAfx.h"
#include <iostream>

int main()
{
    bool all_passed = true;

    std::cout << "Running ControlParams conversion tests...\n\n";

    // Test 1: FromCFPARA
    if (control::test::test_from_cfpara())
    {
        std::cout << "✓ FromCFPARA test passed\n";
    }
    else
    {
        std::cout << "✗ FromCFPARA test FAILED\n";
        all_passed = false;
    }

    // Test 2: ToCFPARA
    if (control::test::test_to_cfpara())
    {
        std::cout << "✓ ToCFPARA test passed\n";
    }
    else
    {
        std::cout << "✗ ToCFPARA test FAILED\n";
        all_passed = false;
    }

    // Test 3: Round-trip conversion
    if (control::test::test_roundtrip())
    {
        std::cout << "✓ Round-trip conversion test passed\n";
    }
    else
    {
        std::cout << "✗ Round-trip conversion test FAILED\n";
        all_passed = false;
    }

    // Test 4: Default values
    if (control::test::test_default_values())
    {
        std::cout << "✓ Default values test passed\n";
    }
    else
    {
        std::cout << "✗ Default values test FAILED\n";
        all_passed = false;
    }

    std::cout << "\n";

    if (all_passed)
    {
        std::cout << "All tests passed successfully!\n";
        return 0;
    }
    else
    {
        std::cout << "Some tests failed.\n";
        return 1;
    }
}
