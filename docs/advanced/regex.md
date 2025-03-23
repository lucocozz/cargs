# Regular Expressions

cargs offers powerful regular expression support via PCRE2 (Perl Compatible Regular Expressions) to validate user inputs.

## Predefined Patterns

cargs provides a wide range of predefined patterns in `cargs/regex.h`:

### Network and Communication

```c
// IPv4: 0-255.0-255.0-255.0-255
CARGS_RE_IPV4

// IPv4 with optional CIDR suffix: 192.168.1.0/24
CARGS_RE_IP4CIDR

// IPv6 (simplified version)
CARGS_RE_IPV6

// MAC Address (with : or - separators) 
CARGS_RE_MAC

// Fully Qualified Domain Name (FQDN)
CARGS_RE_DOMAIN

// URL (supports many protocols)
CARGS_RE_URL

// Specific HTTP(S) URLs
CARGS_RE_HTTP

// File URL
CARGS_RE_FILE_URL

// Port number (1-65535)
CARGS_RE_PORT
```

### Email Addresses

```c
// Basic email validation
CARGS_RE_EMAIL

// Strict email validation (RFC compliant)
CARGS_RE_EMAIL_STRICT
```

### Dates and Times

```c
// ISO 8601 date (YYYY-MM-DD)
CARGS_RE_ISO_DATE

// ISO 8601 date and time (YYYY-MM-DDThh:mm:ss)
CARGS_RE_ISOTIME

// MM/DD/YYYY format
CARGS_RE_US_DATE

// DD/MM/YYYY format
CARGS_RE_EU_DATE

// Time (24h format)
CARGS_RE_TIME24
```

### Phone Numbers

```c
// International phone number
CARGS_RE_PHONE_INTL

// US phone number
CARGS_RE_PHONE_US

// European phone number (general pattern)
CARGS_RE_PHONE_EU
```

### Identity and Security

```c
// Username
CARGS_RE_USER

// Simple password
CARGS_RE_PASSWD

// Strong password
CARGS_RE_PASSWD_STRONG

// UUID (version 4)
CARGS_RE_UUID
```

### Geographic Data

```c
// US ZIP code
CARGS_RE_ZIP

// UK postal code
CARGS_RE_UK_POST

// Canadian postal code
CARGS_RE_CA_POST

// Latitude (-90 to 90)
CARGS_RE_LATITUDE

// Longitude (-180 to 180)
CARGS_RE_LONGITUDE
```

### Files and Paths

```c
// Unix absolute path
CARGS_RE_UNIX_PATH

// Windows absolute path (with drive letter)
CARGS_RE_WIN_PATH

// Filename with extension
CARGS_RE_FILENAME
```

### Numbers and Codes

```c
// Hexadecimal color (e.g., #FF00FF)
CARGS_RE_HEX_COLOR

// RGB color (e.g., rgb(255,0,255))
CARGS_RE_RGB

// Semantic version (major.minor.patch)
CARGS_RE_SEMVER

// Positive integer
CARGS_RE_POS_INT

// Negative integer
CARGS_RE_NEG_INT

// Floating-point number
CARGS_RE_FLOAT

// Hexadecimal number with 0x prefix
CARGS_RE_HEX
```

## Using Predefined Patterns

```c
#include "cargs/regex.h"

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Email validation
    OPTION_STRING('e', "email", "Email address",
                REGEX(CARGS_RE_EMAIL)),
    
    // IP address validation
    OPTION_STRING('i', "ip", "IP address",
                REGEX(CARGS_RE_IPV4)),
    
    // Date validation
    OPTION_STRING('d', "date", "Date (YYYY-MM-DD)",
                REGEX(CARGS_RE_ISO_DATE)),
    
    // Phone number validation
    OPTION_STRING('p', "phone", "Phone number",
                REGEX(CARGS_RE_PHONE_INTL))
)
```

## Defining Custom Patterns

You can define your own patterns with the `MAKE_REGEX` macro:

```c
// Define a custom pattern for product IDs
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")

// Define a custom pattern for simple names
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, digits, underscore, dash")

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Use a custom pattern
    OPTION_STRING('p', "product", "Product ID (format: XX0000-XXXXXX)",
                REGEX(RE_PRODUCT_ID)),
    
    // Use another custom pattern
    OPTION_STRING('n', "name", "Username",
                REGEX(RE_SIMPLE_NAME))
)
```

You can also define inline patterns without using a constant:

```c
OPTION_STRING('z', "zipcode", "US ZIP code",
            REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format: 12345 or 12345-6789")))
```

## MAKE_REGEX Structure

The `MAKE_REGEX` macro takes two parameters:

1. The regular expression pattern
2. An explanation text that will be displayed in case of validation failure

```c
MAKE_REGEX(pattern, explanation)
```

## Custom Error Messages

The explanation provided in `MAKE_REGEX` is used to generate helpful error messages:

```
my_program: Invalid value 'abc123-XYZ': Format: XX0000-XXXXXX
```

## Regular Expression Tips

1. **Escaping**: Remember to double escape characters in C strings (`\\d` instead of `\d`)

2. **Anchors**: Use `^` and `$` to force matching the entire string

3. **Testing**: Test your regular expressions before using them

4. **Complexity**: Prefer multiple simple regular expressions to a single complex one

## Complete Example

See `examples/regex.c` for a complete example of using regular expressions with cargs.
