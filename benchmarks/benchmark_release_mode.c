#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "cargs.h"

// Test with a large and complex options structure to make the 
// performance difference more noticeable
CARGS_OPTIONS(
    complex_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    GROUP_START("Input Options", GROUP_DESC("Options related to input")),
        OPTION_FLAG('v', "verbose", "Enable verbose output"),
        OPTION_STRING('i', "input", "Input file", DEFAULT("input.txt")),
        OPTION_ARRAY_STRING('I', "include", "Include files", FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    GROUP_END(),
    
    GROUP_START("Output Options", GROUP_DESC("Options related to output")),
        OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
        OPTION_STRING('f', "format", "Output format", CHOICES_STRING("text", "json", "xml", "binary")),
        OPTION_FLAG('s', "silent", "Suppress output", CONFLICTS("verbose")),
    GROUP_END(),
    
    GROUP_START("Processing Options", GROUP_DESC("Options controlling processing")),
        OPTION_INT('l', "level", "Processing level", RANGE(1, 10), DEFAULT(5)),
        OPTION_INT('j', "jobs", "Number of parallel jobs", RANGE(1, 100), DEFAULT(4)),
        OPTION_FLOAT('t', "threshold", "Processing threshold", DEFAULT(0.5)),
        OPTION_MAP_STRING('D', "define", "Define variables", FLAGS(FLAG_SORTED_KEY)),
    GROUP_END(),
    
    GROUP_START("Advanced Options", GROUP_DESC("Advanced configuration")),
        OPTION_FLAG('d', "debug", "Enable debug mode"),
        OPTION_STRING('c', "config", "Configuration file"),
        OPTION_ARRAY_INT('p', "ports", "Port numbers", FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
        OPTION_MAP_INT('m', "memory", "Memory limits", FLAGS(FLAG_SORTED_KEY)),
    GROUP_END(),
    
    POSITIONAL_STRING("command", "Command to execute"),
    POSITIONAL_STRING("arguments", "Command arguments", FLAGS(FLAG_OPTIONAL))
)

// Invalid options with duplicated option names
CARGS_OPTIONS(
    invalid_options,
    OPTION_FLAG('v', "verbose", "Verbose output"),
    OPTION_STRING('v', "verbose", "Duplicate option")  // Same names - should fail validation
)

// Small options structure for comparison
CARGS_OPTIONS(
    simple_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    OPTION_STRING('o', "output", "Output file"),
    POSITIONAL_STRING("input", "Input file")
)

// Forward declaration of the function
cargs_t cargs_init_mode(cargs_option_t *options, const char *program_name, const char *version, bool release_mode);

// Measure initialization time for both modes
double measure_init_time(cargs_option_t *options, const char *program_name, 
                         const char *version, bool release_mode, int iterations) 
{
    clock_t start, end;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; i++) {
        start = clock();
        
        // Initialize cargs with the specified mode
        cargs_t cargs = cargs_init_mode(options, program_name, version, release_mode);
        
        end = clock();
        total_time += ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Clean up
        cargs_free(&cargs);
    }
    
    return total_time / iterations;
}

void run_benchmark(bool release_mode) 
{
    const int warmup_iterations = 100;
    const int measurement_iterations = 10000;
    
    // Warm-up iterations for more stable results
    measure_init_time(simple_options, "test_program", "1.0.0", release_mode, warmup_iterations);
    
    // Measure initialization times
    double time_simple = measure_init_time(simple_options, "test_program", "1.0.0", 
                                          release_mode, measurement_iterations);
    
    double time_complex = measure_init_time(complex_options, "test_program", "1.0.0", 
                                           release_mode, measurement_iterations);
    
    // Only measure invalid options in release mode (would crash in debug mode)
    double time_invalid = 0.0;
    if (release_mode) {
        time_invalid = measure_init_time(invalid_options, "test_program", "1.0.0", 
                                        release_mode, measurement_iterations);
    }
    
    // Print results
    printf("Simple options structure (%d iterations):\n", measurement_iterations);
    printf("  Average time: %.9f seconds per initialization\n", time_simple);
    
    printf("\nComplex options structure (%d iterations):\n", measurement_iterations);
    printf("  Average time: %.9f seconds per initialization\n", time_complex);
    printf("  Ratio to simple: %.2fx slower\n", time_complex / time_simple);
    
    if (release_mode) {
        printf("\nInvalid options structure (%d iterations):\n", measurement_iterations);
        printf("  Average time: %.9f seconds per initialization\n", time_invalid);
        printf("  Ratio to simple: %.2fx\n", time_invalid / time_simple);
    }
}

int main(int argc, char **argv) 
{
    // Command line argument to select mode:
    // - 0 or no args: Run in debug mode (validation enabled)
    // - 1: Run in release mode (validation disabled)
    // - any other value: Run both modes
    
    bool release_mode_only = false;
    bool debug_mode_only = false;
    
    if (argc > 1) {
        int mode = atoi(argv[1]);
        release_mode_only = (mode == 1);
        debug_mode_only = (mode == 0);
    }
    
    // Run appropriate mode(s)
    if (!release_mode_only) {
        run_benchmark(false);  // Debug mode (validation enabled)
    }
    
    if (!debug_mode_only) {
        run_benchmark(true);   // Release mode (validation disabled)
    }
    
    return 0;
}
