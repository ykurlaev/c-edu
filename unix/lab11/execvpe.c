extern char * const *environ;

int execvpe(const char *file, char * const argv[], char * const envp[])
{
  environ=envp;
  return execvp(file,argv);
}