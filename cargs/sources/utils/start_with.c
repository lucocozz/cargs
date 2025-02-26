#include <stdbool.h>
#include <stddef.h>

bool	start_with(const char *start_with, const char *str)
{
	size_t	i = 0;

	while (start_with[i] != '\0' && str[i] != '\0')
	{
		if (start_with[i] != str[i])
			return (false);
		++i;
	}
	return (start_with[i] == '\0');
}
