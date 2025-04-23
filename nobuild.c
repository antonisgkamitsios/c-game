#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "nobuild.h"

typedef struct
{
  char **data;
  int length;
  int capacity;
} Cmd;

typedef struct
{
  char *data;
  int length;
  int capacity;
} String;

#define INVALID_PROC -1


void cmd_append_null(Cmd *cmd, ...)
{
  va_list args;

  va_start(args, cmd);

  char *arg = va_arg(args, char *);
  while (arg != NULL)
  {
    da_append(cmd, arg);
    arg = va_arg(args, char *);
  }

  va_end(args);
}
#define cmd_append(cmd, ...) cmd_append_null(cmd, __VA_ARGS__, NULL)

void cmd_render(Cmd cmd, String *s)
{
  for (size_t i = 0; i < cmd.length; i++)
  {
    const char *cmd_val = cmd.data[i];
    size_t n = strlen(cmd_val);

    da_append_many(s, cmd_val, n);
    da_append_many(s, " ", 1);
  }
}

pid_t cmd_run_async(Cmd cmd)
{
  String s = {0};
  cmd_render(cmd, &s);
  fprintf(stdout, "CMD: %s\n", s.data);

  pid_t cpid = fork();
  if (cpid < 0)
  {
    fprintf(stderr, "ERROR: could not fork child process: %s\n", strerror(errno));
    return INVALID_PROC;
  }

  if (cpid == 0)
  {
    if (execvp(cmd.data[0], cmd.data) < 0)
    {
      fprintf(stderr, "ERROR: cannot execute command\n");
      exit(1);
    }
  }
  return cpid;
}
bool wait_for(pid_t pid)
{
  for (;;)
  {
    int status;
    if (waitpid(pid, &status, 0) < 0)
    {
      fprintf(stderr, "ERROR: cannot wait on command (pid %d): %s\n", pid, strerror(errno));
      return false;
    }

    if (WIFEXITED(status))
    {
      int exit_status = WEXITSTATUS(status);
      if (exit_status != 0)
      {
        fprintf(stderr, "ERROR: command exited with exit code %d", exit_status);
        return false;
      }

      break;
    }

    if (WIFSIGNALED(status))
    {
      fprintf(stderr, "ERROR: command pricess was terminated by %s", strsignal(WTERMSIG(status)));
      return false;
    }
  }
  return true;
}

bool cmd_run_sync(Cmd cmd)
{
  pid_t pid = cmd_run_async(cmd);
  if (pid == INVALID_PROC)
    return false;

  return wait_for(pid);
}

int main()
{
  Cmd cmd = {0};
  cmd_append(&cmd, "gcc", "-o", "main");
  cmd_append(&cmd, "foo bar baz.c");

  if (!cmd_run_sync(cmd))
    return 1    waitpid(pic, &status, 0);
  };

  return 0;
}