#ifndef CARGS_CALLBACKS_HANDLERS_H
#define CARGS_CALLBACKS_HANDLERS_H


typedef struct cargs_s cargs_t;

void	string_handler(cargs_option_t *options, char *arg);
void	int_handler(cargs_option_t *options, char *arg);
void	bool_handler(cargs_option_t *options, char *arg);
void	float_handler(cargs_option_t *options, char *arg);

void	help_handler(cargs_option_t *options, char *arg);
void	version_handler(cargs_option_t *options, char *arg);


#endif // CARGS_CALLBACKS_HANDLERS_H
