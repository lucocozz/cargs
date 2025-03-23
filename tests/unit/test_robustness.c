#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "cargs.h"

CARGS_OPTIONS(
    robust_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_INT('i', "int", "Integer option", RANGE(1, 100)),
    OPTION_STRING('r', "required", "Required option", FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('c', "choice", "Choice option", CHOICES_STRING("one", "two", "three")),
    OPTION_FLAG('a', "flag-a", "Flag A", CONFLICTS("flag-b")),
    OPTION_FLAG('b', "flag-b", "Flag B", CONFLICTS("flag-a"))
)

void setup_error_capture(void) {
    cr_redirect_stderr();
}

Test(robustness, out_of_range, .init = setup_error_capture) {
    char *argv[] = {"program", "--int=101"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    cargs_t cargs = cargs_init(robust_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS);
    cargs_free(&cargs);
}

Test(robustness, missing_required, .init = setup_error_capture) {
    char *argv[] = {"program", "--int=50"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    cargs_t cargs = cargs_init(robust_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS);
    cargs_free(&cargs);
}

Test(robustness, invalid_choice, .init = setup_error_capture) {
    char *argv[] = {"program", "--required=value", "--choice=invalid"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    cargs_t cargs = cargs_init(robust_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS);
    cargs_free(&cargs);
}

Test(robustness, conflicting_options, .init = setup_error_capture) {
    char *argv[] = {"program", "--required=value", "--flag-a", "--flag-b"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    cargs_t cargs = cargs_init(robust_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS);
    cargs_free(&cargs);
}

Test(robustness, unknown_option, .init = setup_error_capture) {
    char *argv[] = {"program", "--required=value", "--unknown=value"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    cargs_t cargs = cargs_init(robust_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS);
    cargs_free(&cargs);
}
