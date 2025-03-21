#include "cargs/internal/utils.h"
#include "cargs/types.h"

value_t cargs_get(cargs_t cargs, const char *option_path)
{
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);
    if (option == NULL)
        return ((value_t){.raw = 0});
    return (option->value);
}

bool cargs_is_set(cargs_t cargs, const char *option_path)
{
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);
    if (option == NULL)
        return (false);
    return (option->is_set);
}

size_t cargs_count(cargs_t cargs, const char *option_path)
{
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);
    if (option == NULL)
        return (0);
    return (option->value_count);
}

value_t cargs_array_get(cargs_t cargs, const char *option_path, size_t index)
{
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);

    if (option == NULL)
        return ((value_t){.raw = 0});

    // Check if the option is an array type
    if (!(option->value_type & VALUE_TYPE_ARRAY))
        return ((value_t){.raw = 0});

    // Check if the index is valid
    if (index >= option->value_count)
        return ((value_t){.raw = 0});

    // Return the element at the specified index
    return option->value.as_array[index];
}

value_t cargs_map_get(cargs_t cargs, const char *option_path, const char *key)
{
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);

    if (option == NULL)
        return ((value_t){.raw = 0});

    // Check if the option is a map type
    if (!(option->value_type & VALUE_TYPE_MAP))
        return ((value_t){.raw = 0});

    // Look for the key in the map
    for (size_t i = 0; i < option->value_count; ++i) {
        if (option->value.as_map[i].key && strcmp(option->value.as_map[i].key, key) == 0) {
            return option->value.as_map[i].value;
        }
    }

    // Key not found
    return ((value_t){.raw = 0});
}

cargs_array_it_t cargs_array_it(cargs_t cargs, const char *option_path)
{
    cargs_array_it_t it     = {0};
    cargs_option_t  *option = find_option_by_active_path(cargs, option_path);

    if (option == NULL || !(option->value_type & VALUE_TYPE_ARRAY))
        return it;  // Return empty iterator

    it._array    = option->value.as_array;
    it._count    = option->value_count;
    it._position = 0;
    return it;
}

bool cargs_array_next(cargs_array_it_t *it)
{
    if (it == NULL || it->_position >= it->_count)
        return false;

    it->value = it->_array[it->_position++];
    return true;
}

void cargs_array_reset(cargs_array_it_t *it)
{
    if (it != NULL)
        it->_position = 0;
}

cargs_map_it_t cargs_map_it(cargs_t cargs, const char *option_path)
{
    cargs_map_it_t  it     = {0};
    cargs_option_t *option = find_option_by_active_path(cargs, option_path);

    if (option == NULL || !(option->value_type & VALUE_TYPE_MAP))
        return it;  // Return empty iterator

    it._map      = option->value.as_map;
    it._count    = option->value_count;
    it._position = 0;
    return it;
}

bool cargs_map_next(cargs_map_it_t *it, const char **key, value_t *value)
{
    if (it == NULL || it->_position >= it->_count)
        return false;

    cargs_pair_t pair = it->_map[it->_position++];

    if (key != NULL)
        it->key = pair.key;

    if (value != NULL)
        it->value = pair.value;

    return (true);
}

void cargs_map_reset(cargs_map_it_t *it)
{
    if (it != NULL)
        it->_position = 0;
}
