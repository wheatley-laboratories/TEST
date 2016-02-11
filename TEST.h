/**
 * Copyright (C) 2016 Roland Hieber <rohieb@rohieb.name>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __TEST_H_INCLUDE_GUARD__
#define __TEST_H_INCLUDE_GUARD__

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_ITALIC "\x1b[3m"
#define ANSI_RESET "\x1b[0m"

static size_t __test_asserts = 0;
static size_t __test_testcases = 0;
static size_t __test_failed_testcases = 0;
static size_t __test_failed_asserts = 0;
static int __test_last_assert_failed = 0;
static int __test_any_assert_in_this_testcase_failed = 0;
static int __test_last_testcase_failed = 0;
static size_t __test_data_index = -1;

/* some helper macros */
#define __TEST_PRINT_TESTCHAMBER_HEADER(name) \
	printf("\n" ANSI_BLUE "TESTCHAMBER " name ":" ANSI_RESET "\n")
#define __TEST_PRINT_TESTCHAMBER_RESULT(name) \
	if (!__test_last_testcase_failed) { \
		printf(ANSI_GREEN "✔ PASS" ANSI_RESET "\n"); \
	}
#define __TEST_PRINT_ASSERT(expr) \
	printf(ANSI_RED "✘ FAIL:" ANSI_RESET " " __FILE__ ":%d\t" expr "\n", __LINE__)
#define __TEST_PRINT_VALUE(name, x) \
	if (sizeof(x) == sizeof(char)) { \
		printf("  \t%s: \t%d \t0x%x \t'%c'\n", name, (x), (x), (x)); \
	} else { \
		printf("  \t%s: \t%d \t0x%x\n", name, (x), (x)); \
	}
#define __TEST_PRINT_VALUE_STRING(name, x) \
		printf("  \t%s: \t'%s'\n", name, (x));
#define __TEST_PRINT_DATA_INDEX() \
	if (__test_data_index != -1) { \
		printf("  \tat Data Index %d:\n", __test_data_index); \
	}

#define __TEST_TESTCHAMBER_INIT() \
	__test_last_testcase_failed = 0; \
	__test_any_assert_in_this_testcase_failed = 0; \
	__test_testcases++
#define __TEST_TESTCHAMBER_FAILED() \
	__test_last_testcase_failed = 1; \
	__test_failed_testcases++

#define __TEST_ASSERT_INIT() \
	__test_last_assert_failed = 0; \
	__test_asserts++
#define __TEST_ASSERT_FAILED() \
	__test_last_assert_failed = 1; \
	__test_any_assert_in_this_testcase_failed = 1; \
	__test_failed_asserts++

/** Define or declare a Test Case. */
#define TEST_CHAMBER(func) void __test_##func()

/** Run a Test Case. */
#define TEST_CHAMBER_RUN(func) { \
	__TEST_TESTCHAMBER_INIT(); \
	__TEST_PRINT_TESTCHAMBER_HEADER(#func); \
	__test_##func(); \
	if (__test_any_assert_in_this_testcase_failed) { \
		__TEST_TESTCHAMBER_FAILED(); \
	} \
	__TEST_PRINT_TESTCHAMBER_RESULT(#func); \
}


/**
 * Set Data Index. Useful if your test code loops over an array of test data
 * and you want to know at which element it fails.
 * If used inside a block, the Data Index is reset automatically at the end of
 * the block. To unset it explicitely, see TEST_UNSET_DATA_INDEX().
 */
#define TEST_SET_DATA_INDEX(n) \
	int __test_data_index = (n)
	/* Implementation Note: we're shadowing the global variable here to make use
	 * of automatic block scoping. The global variable exists only so that the
	 * TEST_PRINT_DATA_INDEX() call doesn't fail if no Data Index was set. */

/** Explicitely unset Data Index. */
#define TEST_UNSET_DATA_INDEX(n) \
	__test_data_index = -1

/** Print a message without asserting. Understands printf syntax. */
#define TEST_PRINTF(...) \
	printf(ANSI_CYAN "  MSG: " ANSI_RESET " "); \
	printf(__VA_ARGS__); \
	printf("\n")

/** Assert to true */
#define TEST_ASSERT(x) \
	__TEST_ASSERT_INIT(); \
	if (!(x)) { \
		__TEST_PRINT_ASSERT("ASSERT(" #x ")"); \
		__TEST_PRINT_DATA_INDEX(); \
		__TEST_PRINT_VALUE("value", x); \
		__TEST_ASSERT_FAILED(); \
	}

/** Assert to x == y */
#define TEST_EQUAL(x, y) \
	__TEST_ASSERT_INIT(); \
	if ((x) != (y)) { \
		__TEST_PRINT_ASSERT("EQUAL(" #x "," #y ")"); \
		__TEST_PRINT_DATA_INDEX(); \
		__TEST_PRINT_VALUE("lhs", x); \
		__TEST_PRINT_VALUE("rhs", y); \
		__TEST_ASSERT_FAILED(); \
	}

/** Assert if strings are equal */
#define TEST_STRING_EQUAL(str1, str2) \
	__TEST_ASSERT_INIT(); \
	if (strcmp((str1), (str2)) != 0) { \
		__TEST_PRINT_ASSERT("STRING_EQUAL(" #str1 "," #str2 ")"); \
		__TEST_PRINT_DATA_INDEX(); \
		__TEST_PRINT_VALUE_STRING("str1", str1); \
		__TEST_PRINT_VALUE_STRING("str2", str2); \
		__TEST_ASSERT_FAILED(); \
	}

/** Assert if vectors or strings are equal for the first n elements.
 * Can be used on anything that supports bracket operator syntax. */
#define TEST_ELEMENTS_EQUAL(x, y, n) { \
	__TEST_ASSERT_INIT(); \
	size_t index; \
	for (index = 0; index < n; index++) { \
		if ((x)[index] != (y)[index]) { \
			__TEST_PRINT_ASSERT("ELEMENTS_EQUAL(" #x "," #y "," #n ")"); \
			__TEST_PRINT_DATA_INDEX(); \
			__TEST_PRINT_VALUE("elem_index", index); \
			__TEST_PRINT_VALUE("lhs[index]", (x)[index]); \
			__TEST_PRINT_VALUE("rhs[index]", (y)[index]); \
			__TEST_ASSERT_FAILED(); \
			break; \
		} \
	} \
}

/** Print the test results */
#define TEST_RESULTS() \
	printf("\nWell done. Here come the test results:\n"); \
	printf("\tTotal:  %5d tests, %5d asserts\n", __test_testcases, \
			__test_asserts); \
	printf("\tFailed: %s%5d%s tests, %s%5d%s asserts\n", \
			(__test_failed_testcases > 0) ? ANSI_RED : ANSI_GREEN, \
				__test_failed_testcases, ANSI_RESET, \
			(__test_failed_asserts > 0) ? ANSI_RED : ANSI_GREEN, \
				__test_failed_asserts, ANSI_RESET \
		); \
	if (__test_failed_asserts > 0 || __test_failed_testcases > 0) { \
		printf(ANSI_ITALIC "\n" \
		"  »You are a horrible person. That's what it says: a horrible person.\n" \
		"   We weren't even testing for that.«\n" \
		"                                                  -- GLaDOS, Portal 2" \
		ANSI_RESET "\n\n"); \
	} else { \
		printf(ANSI_ITALIC "\n" \
		"  »Great work! Because this message is prerecorded, any observations\n" \
		"   related to your performance are speculation on our part. Please\n" \
		"   disregard any undeserved compliments.«\n" \
		"                                               -- Announcer, Portal 2" \
		ANSI_RESET "\n\n"); \
	}

#endif // def __TEST_H_INCLUDE_GUARD__
