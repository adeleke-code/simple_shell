#include "shell.h"

/**
 * findenv - returns the value of an environment variable
 *
 * @env: an array of all environment variables
 * @name: name of the variable to search for
 *
 * Return: the value of the environment variable if found, or
 * NULL if it couldn't be found
 */
char *findenv(char *env[], const char *name)
{
	char *key, *val, *var;
	int i;

	for (i = 0; env[i]; i++)
	{
		var = strdup(env[i]);
		key = strtok(var, "=");
		if (!_strcmp(key, name))
		{
			val = strdup(strtok(NULL, ""));
			free(var);
			return (val);
		}
		free(var);
	}
	return (NULL);
}

/**
 * findfile - searches through a directory for a file
 *
 * @dir: the directory to search
 * @filename: name of the file to search for
 *
 * Return: a pointer to the file if found, else NULL
 */
struct dirent *findfile(DIR *dir, const char *filename)
{
	struct dirent *file;

	errno = 0;
	file = readdir(dir);
	while (file != NULL)
	{
		if (_strcmp(filename, file->d_name) == 0)
			break;
		file = readdir(dir);
	}
	return (file);
}


/**
 * findcmd - searches through $PATH to find a command
 *
 * @command: name of the command
 * @PATH: a string containing directories in which to search
 * for the command. The directories should be delimited by :
 *
 * Return: a pointer to the path of the command if found, otherwise NULL
 */
char *findcmd(const char *command, const char *PATH)
{
	char *dirname, *PATHCPY, *cmd;
	DIR *dir;
	struct dirent *node;

	cmd = NULL;
	PATHCPY = strdup(PATH);
	dirname = strtok(PATHCPY, ":");

	while (dirname)
	{
		dir = opendir(dirname);
		if (!dir)
		{
			dirname = strtok(NULL, ":");
			continue;
		}
		node = findfile(dir, command);
		closedir(dir);
		if (node)
		{
			cmd = joinpath(dirname, node->d_name);
			break;
		}
		dirname = strtok(NULL, ":");
	}
	free(PATHCPY);
	return (cmd);
}
