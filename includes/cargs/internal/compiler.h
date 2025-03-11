/**
 * cargs/internal/compiler.h - Compiler-specific macros and utilities
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines macros for handling compiler-specific behavior.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_INTERNAL_COMPILER_H
 #define CARGS_INTERNAL_COMPILER_H
 
 /**
  * Macros to disable and restore warnings for initializer overrides
  * Used primarily for array initializations in the CARGS_OPTIONS macro
  */
 #if defined(__clang__)
	 #define PRAGMA_DISABLE_OVERRIDE() \
		 _Pragma("clang diagnostic push") \
		 _Pragma("clang diagnostic ignored \"-Winitializer-overrides\"")
 
	 #define PRAGMA_RESTORE() \
		 _Pragma("clang diagnostic pop")
 
 #elif defined(__GNUC__)
	 #define PRAGMA_DISABLE_OVERRIDE() \
		 _Pragma("GCC diagnostic push") \
		 _Pragma("GCC diagnostic ignored \"-Woverride-init\"")
 
	 #define PRAGMA_RESTORE() \
		 _Pragma("GCC diagnostic pop")
 
 #else
	 /* Fallback for other compilers */
	 #define PRAGMA_DISABLE_OVERRIDE()
	 #define PRAGMA_RESTORE()
 #endif
 
 #endif /* CARGS_INTERNAL_COMPILER_H */
