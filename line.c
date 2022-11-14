#include "shell.h"

/**
 * getlines - read all lines from stdin.
 *
 * Description: When file is terminal, this function
 * acts like getline(3), reading just one line.
 * When file is a pipe or regular file, all the content is
 * consumed at a go.
 *
 * @fd: the file descriptor
 *
 * Return: a buffer holding all contents of the file.
 * NULL is returned if malloc failed or if EOF is reached.
 */
char *getlines(int fd)
{
	char *buffer;
	size_t offset, size;
	int step;
	ssize_t bytesread;

	size = 1024;
	buffer = malloc(size);
	offset = 0;
	step = size;

	if (!buffer)
		return (NULL);

	while (true)
	{
		bytesread = read(fd, &buffer[offset], step);
		if (bytesread == 0)
		{
			free(buffer);
			return (NULL);
		}
		if (bytesread < step)
			break;
		offset += step;
		buffer = _realloc(buffer, size, size + step);
		size += step;
		if (!buffer)
			return (NULL);
	}
	buffer[offset + bytesread - 1] = '\0';
	return (buffer);
}


/**
 * getLine - read a line from the stdin
 * Return: the line read from stdin, or NULL if EOF was encountered
 *
 * NOTE: the newline would not be include in the returned string
 */
char *getLine(void)
{
	int index;
	size_t bufsize;
	char *buffer;
	int c;

	bufsize = 2;
	buffer = malloc(bufsize);
	index = 0;

	if (buffer == NULL)
	{
		perror("Error");
		exit(errno);
	}
	c = fgetc(stdin);

	while (c != EOF)
	{
		if (c == '\n')
		{
			appendChar(&buffer, &bufsize, '\0', index);
			break;
		}
		appendChar(&buffer, &bufsize, c, index);
		index++;
		c = fgetc(stdin);
	}
	if (c == EOF)
	{
		free(buffer);
		return (NULL);
	}
	return (buffer);
}

/**
 * split - split a string using a delimiter
 *
 * @string: the string to split
 *
 * @delimiter: a list of delimiters to use to split the string
 *
 * @max: the maximum number of times the string should be splitted
 * Set this to 0 to split infinitely
 *
 * Return: A null-terminated array of strings.
 * NULL is returned if malloc failed or if the string
 * could not be split; this could be because:
 * - the string is empty
 * - the string is NULL
 * - the string only contains delimiters
 * - delimiters is NULL
 * - delimiters is empty
 */
char **split(char *string, char *delimiter, unsigned int max)
{
	char **array, prev, curr;
	size_t arr_size;
	unsigned int arr_ind, str_ind, i;
	bool flipped;

	if (!(string && *string && delimiter && *delimiter))
		return (NULL);

	arr_ind = 0;
	arr_size = 2;
	array = malloc(arr_size * sizeof(char *));
	prev = '\0';

	for (str_ind = 0; string[str_ind]; str_ind++)
	{
		curr = string[str_ind];
		flipped = false;
		for (i = 0; delimiter[i]; i++)
			if (curr == delimiter[i])
			{
				string[str_ind] = '\0';
				flipped = true;
				break;
			}
		if (!prev && !flipped)
			appendStr(&array, &arr_size, &string[str_ind], arr_ind++);
		if (max && max + 1 == arr_ind)
			break;
		prev = string[str_ind];
	}
	if (arr_ind)
		appendStr(&array, &arr_size, NULL, arr_ind);
	else
	{
		free(array);
		array = NULL;
	}
	return (array);
}

/**
 * tokenizeLine - split a line into separate words
 * @line: a malloced string to tokenize
 * Return: a null-terminated array of strings
 */
char **tokenizeLine(char *line)
{
	char **array, c, temp;
	size_t arr_size;
	int arr_index, line_index;

	if (!(line && *line))
		return (NULL);

	line_index = arr_index = 0; /* line index */
	arr_size = 4;
	array = malloc(arr_size * sizeof(char *));
	temp = '\0';

	for (line_index = 0; line && line[line_index]; line_index++)
	{
		c = line[line_index];
		if (c == ' ' || c == '\t')
			line[line_index] = '\0';
		else
			if (temp == '\0')
				appendStr(&array, &arr_size, &line[line_index], arr_index++);
		temp = line[line_index];
	}
	if (arr_index)
		appendStr(&array, &arr_size, NULL, arr_index);
	else
	{
		free(array);
		array = NULL;
	}
	return (array);
}
