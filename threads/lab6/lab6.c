#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 1024

int copy_file(char *src, char *dst)
{
  int src_fd, dst_fd;
  for(;;)
  {
    src_fd = open(src,O_RDONLY);
    if(src_fd==-1)
    {
      if(errno==EMFILE)
      {
        sleep(1);
        continue;
      }
      else
      {
        perror(src);
      }
    }
    break;
  }
  for(;;)
  {
    dst_fd = open(dst,O_WRONLY|O_CREAT|O_EXCL,0644);
    if(dst_fd==-1)
    {
      if(errno==EMFILE)
      {
        sleep(1);
        continue;
      }
      else
      {
        perror(dst);
        return -1;
      }
    }
    break;
  }
  char buffer[BUF_SIZE];
  int e;
  while((e=read(src_fd,buffer,BUF_SIZE))!=0)
  {
    if(e==-1)
    {
      perror(src);
      break;
    }
    if((e=write(dst_fd,buffer,e))==-1)
    {
      perror(dst);
      break;
    }
  }
  if(close(src_fd)==-1)
  {
    perror(src);
  }
  if(close(dst_fd)==-1)
  {
    perror(dst);
  }
  return e;
}

void *copy_thread(void *arg)
{
  int err = 0;
  char **names = (char **)arg;
  char *src = names[0];
  char *dst = names[1];
  DIR *dir;
  for(;;)
  {
    dir = opendir(src);
    if(dir==NULL)
    {
      if(errno==EMFILE)
      {
        sleep(1);
        continue;
      }
      else
      {
        perror(src);
        pthread_exit((void *)-1);
      }
    }
    break;
  }
  struct dirent *direntry = malloc(sizeof(struct dirent)+pathconf(src,_PC_NAME_MAX)+1);
  if(readdir==NULL)
  {
    perror("malloc");
    closedir(dir);
    free(src);
    free(dst);
    free(arg);
    pthread_exit((void *)-1);
  }
  struct dirent *res = NULL;
  struct dirent **result = &res;
  readdir_r(dir,direntry,result);
  while(res!=NULL)
  {
    if((direntry->d_type==DT_DIR && strcmp(direntry->d_name,".") && strcmp(direntry->d_name,"..")) || direntry->d_type==DT_REG)
    {
      char *new_src = malloc(strlen(src)+strlen(direntry->d_name)+2);
      if(new_src==NULL)
      {
        perror("malloc");
        err = -1;
        break;
      }
      new_src[0] = '\0';
      strcat(new_src,src);
      strcat(new_src,"/");
      strcat(new_src,direntry->d_name);
      char *new_dst = malloc(strlen(dst)+strlen(direntry->d_name)+2);
      if(new_dst==NULL)
      {
        free(new_src);
        perror("malloc");
        err = -1;
        break;
      }
      new_dst[0] = '\0';
      strcat(new_dst,dst);
      strcat(new_dst,"/");
      strcat(new_dst,direntry->d_name);
      if(direntry->d_type==DT_REG)
      {
        int e = copy_file(new_src,new_dst);
        free(new_src);
        free(new_dst);
        if(e==-1)
        {
          err = -1;
          break;
        }
      }
      else
      {
        if(mkdir(new_dst,0755)==-1)
        {
          free(new_src);
          free(new_dst);
          perror("mkdir");
          err = -1;
          break;
        }
        char **new_arg = malloc(2*sizeof(char **));
        if(new_arg==NULL)
        {
          free(new_src);
          free(new_dst);
          perror("malloc");
          err = -1;
          break;
        }
        new_arg[0] = new_src;
        new_arg[1] = new_dst;
        pthread_t thread;
        int e;
        for(;;)
        {
          if((e=pthread_create(&thread,NULL,copy_thread,new_arg))!=0)
          {
            if(e==EAGAIN)
            {
              usleep(100);
              continue;
            }
            fprintf(stderr,"thread_create: %s\n",strerror(e));
          }
          break;
        }
        if((e=pthread_detach(thread))!=0)
        {
          fprintf(stderr,"thread_create: %s\n",strerror(e));
        }
      }
    }
    readdir_r(dir,direntry,result);
  }
  closedir(dir);
  free(src);
  free(dst);
  free(arg);
  pthread_exit((void *)err);
}

int main(int argc, char *argv[])
{
  if(argc!=3)
  {
    fprintf(stderr,"Invalid argument!\n");
    return 1;
  }
  char *src, *dst;
  if((src=strdup(argv[1]))==NULL)
  {
    perror("strdup");
    return 1;
  }
  if((dst=strdup(argv[2]))==NULL)
  {
    free(src);
    perror("strdup");
    return 1;
  }
  char **arg = malloc(2*sizeof(char **));
  if(arg==NULL)
  {
    free(src);
    free(dst);
    perror("malloc");
    return 1;
  }
  arg[0] = src;
  arg[1] = dst;
  pthread_detach(pthread_self());
  if(copy_thread(arg)!=0)
  {
    return 1;
  }
  return 0;
}
