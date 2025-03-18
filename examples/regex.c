/**
 * Example demonstrating regex validation for cargs
 *
 * Shows a mix of predefined patterns and custom patterns
 */

#include "cargs.h"
#include "cargs/regex.h"  /* Import predefined patterns */
#include <stdio.h>
#include <stdlib.h>


/* Custom regex patterns */
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, numbers, underscore, dash")

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),

    // SECTION 1: Using predefined patterns from header
    GROUP_START("Network and Communication", GROUP_DESC("Network-related options")),
       OPTION_STRING('i', "ip", "IPv4 address",
                    REGEX(CARGS_RE_IPV4)),

       OPTION_STRING('m', "mac", "MAC address",
                    REGEX(CARGS_RE_MAC)),

        OPTION_STRING('e', "email", "Email address",
                    REGEX(CARGS_RE_EMAIL)),

        OPTION_STRING('u', "url", "URL with any protocol",
                    REGEX(CARGS_RE_URL)),
    GROUP_END(),

    // SECTION 2: Custom patterns defined in this file
    GROUP_START("Custom Formats", GROUP_DESC("Options with custom regex patterns")),
        OPTION_STRING('p', "product", "Product ID (format: XX0000-XXXXXX)",
                    REGEX(RE_PRODUCT_ID)),

        OPTION_STRING('n', "name", "Username (letters, numbers, underscore, dash)",
                    REGEX(RE_SIMPLE_NAME)),
    GROUP_END(),

    // SECTION 3: Custom inline regex (without defining a constant)
    GROUP_START("Inline Patterns", GROUP_DESC("Options with inline regex patterns")),
        OPTION_STRING('z', "zipcode", "US Zip code",
                    REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format: 12345 or 12345-6789"))),

        OPTION_STRING('t', "time", "Time (format: HH:MM)",
                    REGEX(MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9]$", "Format: HH:MM"))),

        OPTION_STRING('f', "float", "Floating point number",
                    REGEX(MAKE_REGEX("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$",
                           "Format: 123.45 or 1.23e-4"))),
    GROUP_END(),

    // SECTION 4: Combining predefined patterns with custom options
    GROUP_START("Combined Patterns", GROUP_DESC("Options with combined validation")),
        // Date with custom error message
        OPTION_STRING('d', "date", "Date (YYYY-MM-DD)",
                    REGEX(CARGS_RE_ISO_DATE),
                    HINT("YYYY-MM-DD")),

        // Password with predefined pattern and custom error message
        OPTION_STRING('P', "password", "Password (8+ chars, mixed case, numbers, symbols)",
                    REGEX(CARGS_RE_PASSWD_STRONG),
                    HINT("StrongP@ss1")),

        // Version number with additional flags
        OPTION_STRING('v', "version", "Semantic version",
                    REGEX(CARGS_RE_SEMVER),
                    HINT("X.Y.Z"),
                    FLAGS(FLAG_REQUIRED)),
    GROUP_END()
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "regex_example", "1.0.0");
    cargs.description = "Example of using regex validation with both predefined and custom patterns";

    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS)
        return status;

    printf("Validation successful! All provided values match the expected patterns.\n\n");

    // Display validated values grouped by category
    printf("Network & Communication:\n");
    printf("  IP Address: %s\n", cargs_is_set(cargs, "ip") ?
           cargs_get_value(cargs, "ip").as_string : "(not provided)");
    printf("  MAC Address: %s\n", cargs_is_set(cargs, "mac") ?
           cargs_get_value(cargs, "mac").as_string : "(not provided)");
    printf("  Email: %s\n", cargs_is_set(cargs, "email") ?
           cargs_get_value(cargs, "email").as_string : "(not provided)");
    printf("  URL: %s\n", cargs_is_set(cargs, "url") ?
           cargs_get_value(cargs, "url").as_string : "(not provided)");

    printf("\nCustom Formats:\n");
    printf("  Product ID: %s\n", cargs_is_set(cargs, "product") ?
           cargs_get_value(cargs, "product").as_string : "(not provided)");
    printf("  RGB Color: %s\n", cargs_is_set(cargs, "color") ?
           cargs_get_value(cargs, "color").as_string : "(not provided)");
    printf("  Username: %s\n", cargs_is_set(cargs, "name") ?
           cargs_get_value(cargs, "name").as_string : "(not provided)");

    printf("\nInline Patterns:\n");
    printf("  Zip Code: %s\n", cargs_is_set(cargs, "zipcode") ?
           cargs_get_value(cargs, "zipcode").as_string : "(not provided)");
    printf("  Time: %s\n", cargs_is_set(cargs, "time") ?
           cargs_get_value(cargs, "time").as_string : "(not provided)");
    printf("  Float: %s\n", cargs_is_set(cargs, "float") ?
           cargs_get_value(cargs, "float").as_string : "(not provided)");

    printf("\nCombined Patterns:\n");
    printf("  Date: %s\n", cargs_is_set(cargs, "date") ?
           cargs_get_value(cargs, "date").as_string : "(not provided)");
    printf("  Password: %s\n", cargs_is_set(cargs, "password") ?
           "[HIDDEN]" : "(not provided)");
    printf("  Version: %s\n", cargs_get_value(cargs, "version").as_string);

    cargs_free(&cargs);
    return 0;
}
