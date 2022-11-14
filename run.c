#include "shell.h"

/**
 * runbuiltin - execute a builtin command
 *
 * @self: the shell's state
 * @arguments: command line arguments passed to the shell
 *
 * Return: true(1) if a command was found else false(0)
 */
bool runbuiltin(state *self, char **arguments)
{
	int i, status;
	char *command, **args;
	builtin builtins[] = {
		{"exit", shellexit},
		{"env", shellenv},
		{"setenv", shellsetenv},
		{"unsetenv", shellunsetenv},
		{"cd", shellcd},
		{"alias", shellalias},
		{ NULL, NULL }
	};

	command = arguments[0];
	args = arguments + 1;

	for (i = 0; builtins[i].name; i++)
	{
		if (!_strcmp(builtins[i].name, command))
		{
			status = builtins[i].handler(self, args);
			self->_errno = status;
			return (true);
		}
	}
	return (false);
}

/**
 * runprogram - execute a program
 *
 * @self: the shell's state
 * @arguments: command line arguments passed to the shell
 *
 * Return: true(1) if the program was found else false(0)
 */
int runprogram(state *self, char **arguments)
{
	char *path, **env;
	bool free_path = false;
	int status, i;
	struct stat filestat;

	path = arguments[0];

	if (!ispath(path))
	{
		path = findcmd(path,
			set_default(&(self->env), "PATH", "")->val);
		if (!path)
			return (false);
		free_path = true;
	}
	else if (access(path, F_OK) == -1)
		return (false);

	stat(path, &filestat);

	if (access(path, X_OK) == -1 || S_ISREG(filestat.st_mode) == 0)
	{
		fprinterr(format(
			"%s: %d: %s: Permission denied\n",
			self->prog, self->lineno, arguments[0]));
		self->_errno = 126;
	}
	else
	{
		env = to_strarr(self->env);
		status = execute(path, arguments, env);
		for (i = 0; env[i]; i++)
			free(env[i]);
		free(env);
		self->_errno = status;
	}
	if (free_path)
		free(path);

	return (true);
}

/**
 * interactive - runs the shell in interactive mode
 *
 * @self: the shell's state
 *
 * Return: always 0
 */
int interactive(state *self)
{
	bool found;
	int i;

	for (; true; self->lineno++)
	{
		found = false;
		printout("($) ");
		self->line = getlines(STDIN_FILENO);
		if (!self->line)
		{
			printout("\n");
			break;
		}
		self->arguments = split(self->line, "\t ", 0);
		if (!self->arguments || _strcmp(self->arguments[0], "#") == 0)
		{
			cleanup(self);
			continue;
		}
		for (i = 0; self->arguments[i]; i++)
			self->arguments[i] = replace(self, self->arguments[i]);
		comment(self->arguments);
		found = runbuiltin(self, self->arguments);
		if (!found)
			found = runprogram(self, self->arguments);
		if (!found)
		{
			fprinterr(format(
				"%s: %d: %s: not found\n",
				self->prog, self->lineno, self->arguments[0]
			));
			self->_errno = EKEYEXPIRED;
		}
		cleanup(self);
	}
	return (0);
}

/**
 * execute - execute a program in a child process
 * @program: path of the program to execute
 * @args: command line arguments to pass to the program
 * @env: the environme
 * Return: the exit status of the child process
 */
int execute(const char *program, char *args[], char *env[])
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("Error");
		return (1);
	}
	if (pid == 0)
	{
		execve(program, args, env);
		_exit(EXIT_FAILURE);
	}
	else
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid failed");
			return (-1);
		}

		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
	}
	return (status);
}
