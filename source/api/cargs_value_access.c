#include "cargs/internal/utils.h"
#include "cargs/types.h"


value_t	cargs_get_value(cargs_t cargs, const char *option_path)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	if (option == NULL)
		return ((value_t){.raw = 0});
	return (option->value);
}


bool	cargs_is_set(cargs_t cargs, const char *option_path)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	if (option == NULL)
		return (false);
	return (option->is_set);
}

size_t	cargs_get_value_count(cargs_t cargs, const char *option_path)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	if (option == NULL)
		return (0);
	return (option->value_count);
}


value_t cargs_get_array_element(cargs_t cargs, const char *option_path, size_t index)
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

value_t cargs_get_map_value(cargs_t cargs, const char *option_path, const char *key)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	
	if (option == NULL)
		return ((value_t){.raw = 0});
	
	// Check if the option is a map type
	if (!(option->value_type & VALUE_TYPE_MAP))
		return ((value_t){.raw = 0});
	
	// Look for the key in the map
	for (size_t i = 0; i < option->value_count; ++i)
	{
		if (option->value.as_map[i].key && 
		    strcmp(option->value.as_map[i].key, key) == 0) {
			return option->value.as_map[i].value;
		}
	}
	
	// Key not found
	return ((value_t){.raw = 0});
}


cargs_array_iterator_t cargs_array_iterator(cargs_t cargs, const char *option_path)
{
	cargs_array_iterator_t it = {0};
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	
	if (option == NULL || !(option->value_type & VALUE_TYPE_ARRAY))
		return it;  // Return empty iterator
	
	it.array = option->value.as_array;
	it.count = option->value_count;
	it.current = 0;
	return it;
}

bool cargs_array_next(cargs_array_iterator_t *it, value_t *value)
{
	if (it == NULL || value == NULL || it->current >= it->count)
		return false;
	
	*value = it->array[it->current++];
	return true;
}

void cargs_array_reset(cargs_array_iterator_t *it)
{
	if (it != NULL)
		it->current = 0;
}

cargs_map_iterator_t cargs_map_iterator(cargs_t cargs, const char *option_path)
{
	cargs_map_iterator_t it = {0};
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	
	if (option == NULL || !(option->value_type & VALUE_TYPE_MAP))
		return it;  // Return empty iterator
	
	it.map = option->value.as_map;
	it.count = option->value_count;
	it.current = 0;
	return it;
}

bool cargs_map_next(cargs_map_iterator_t *it, const char **key, value_t *value)
{
	if (it == NULL || it->current >= it->count)
		return false;
	
	cargs_pair_t pair = it->map[it->current++];
	
	if (key != NULL)
		*key = pair.key;
	
	if (value != NULL)
		*value = pair.value;
	
	return true;
}

void cargs_map_reset(cargs_map_iterator_t *it)
{
	if (it != NULL)
		it->current = 0;
}
