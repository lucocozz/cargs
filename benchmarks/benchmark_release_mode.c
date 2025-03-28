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

// Global variables to store timing results for comparison
double release_simple_time = 0.0;
double release_complex_time = 0.0;
double release_invalid_time = 0.0;
double normal_simple_time = 0.0;
double normal_complex_time = 0.0;

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
    printf("%s Mode - Simple options structure (%d iterations): %.9f seconds\n", 
           release_mode ? "Release" : "Normal", measurement_iterations, time_simple);
    printf("%s Mode - Complex options structure (%d iterations): %.9f seconds\n", 
           release_mode ? "Release" : "Normal", measurement_iterations, time_complex);
    
    if (release_mode) {
        printf("%s Mode - Invalid options structure (%d iterations): %.9f seconds\n", 
               release_mode ? "Release" : "Normal", measurement_iterations, time_invalid);
    }
    
    // Store results for comparison
    if (release_mode) {
        release_simple_time = time_simple;
        release_complex_time = time_complex;
        release_invalid_time = time_invalid;
    } else {
        normal_simple_time = time_simple;
        normal_complex_time = time_complex;
    }
}

// Function to display comparison between modes
void display_mode_comparison() {
    printf("\n===== PERFORMANCE COMPARISON: NORMAL vs RELEASE MODE =====\n");
    printf("%-20s | %-12s | %-12s | %-12s\n", "Test Case", "Normal (s)", "Release (s)", "Speedup");
    printf("------------------------------------------------------\n");
    printf("%-20s | %-12.9f | %-12.9f | %.2fx\n", 
           "Simple Options", normal_simple_time, release_simple_time, 
           normal_simple_time / release_simple_time);
    printf("%-20s | %-12.9f | %-12.9f | %.2fx\n", 
           "Complex Options", normal_complex_time, release_complex_time, 
           normal_complex_time / release_complex_time);
    printf("%-20s | %-12s | %-12.9f | %s\n", 
           "Invalid Options", "N/A", release_invalid_time, "N/A");
    printf("======================================================\n");
}

int main(int argc, char **argv) 
{
    // Command line argument to select mode:
    // - 0 or no args: Run in debug mode (validation enabled)
    // - 1: Run in release mode (validation disabled)
    // - any other value: Run both modes
    
    bool release_mode_only = false;
    bool debug_mode_only = false;
    bool compare_modes = true;
    
    if (argc > 1) {
        int mode = atoi(argv[1]);
        release_mode_only = (mode == 1);
        debug_mode_only = (mode == 0);
        compare_modes = !(release_mode_only || debug_mode_only);
    }
    
    printf("=== CARGS PERFORMANCE BENCHMARK ===\n\n");
    
    // Run normal mode (debug mode)
    if (!release_mode_only) {
        printf("Running benchmarks in NORMAL mode (validation enabled)...\n");
        run_benchmark(false);  // Normal mode (validation enabled)
        printf("\n");
    }
    
    // Run release mode
    if (!debug_mode_only) {
        printf("Running benchmarks in RELEASE mode (validation disabled)...\n");
        run_benchmark(true);   // Release mode (validation disabled)
        printf("\n");
    }
    
    // Display comparison if both modes were run
    if (compare_modes) {
        display_mode_comparison();
    }
    
    return 0;
}
