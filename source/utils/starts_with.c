#include <stdbool.h>
#include <stddef.h>
#include <string.h>


/**
 * Checks if a string starts with a specific prefix.
 * @param prefix The prefix string to search for.
 * @param str The string to search in.
 * @return A pointer to the first character after the prefix if found,
 *         NULL otherwise or if any parameter is NULL.
 */
char *starts_with(const char *prefix, const char *str)
{
    if (prefix == NULL || str == NULL)
        return (NULL);

    size_t prefix_len = strlen(prefix);
    
    if (strncmp(str, prefix, prefix_len) == 0)
        return ((char *)(str + prefix_len));
    return (NULL);
}
