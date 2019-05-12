#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int check(struct tm tm, char* buffer, char* cmd){
  char space[6][4];
  memset(space[0],'\0',sizeof(space[0]));
  memset(space[1],'\0',sizeof(space[0]));
  memset(space[2],'\0',sizeof(space[0]));
  memset(space[3],'\0',sizeof(space[0]));
  memset(space[4],'\0',sizeof(space[0]));

  int counter = 0;
  int digit = 0;
  int iter = 0 ;
  while(buffer[iter] != '\0' && counter < 5)
  {
    if(buffer[iter]==' '){
      counter++;
      digit = 0;
    }
    else {
      space[counter][digit] = buffer[iter];
      digit++;
    }
    iter++;
  }
  strcpy(cmd,buffer+iter);
  // printf("%s\n",cmd);
  // for(int i=0;i<5;i++){
  //   printf("%s\n",space[i]);
  // }
  char menit[3]; char jam[3]; char tanggal[3]; char bulan[3]; char hari[3];

  sprintf(menit, "%d", tm.tm_min);
  sprintf(jam, "%d", tm.tm_hour);
  sprintf(tanggal, "%d", tm.tm_mday);
  sprintf(bulan, "%d", tm.tm_mon+1);
  sprintf(hari, "%d", tm.tm_wday);

  int flag = 1;

  if(strcmp(menit, space[0]) != 0 && strcmp(space[0], "*") != 0) return 0;
  if(strcmp(jam, space[1]) != 0 && strcmp(space[1], "*") != 0) return 0;
  if(strcmp(tanggal, space[2]) != 0 && strcmp(space[2], "*") != 0) return 0;
  if(strcmp(bulan, space[3]) != 0 && strcmp(space[3], "*") != 0) return 0;
  if(strcmp(hari, space[4]) != 0 && strcmp(space[4], "*") != 0) return 0;

  return flag;
}

void *print_message_function( void *ptr )
{
    char *message;
    message = (char *) ptr;
    // printf("%s\n",message);
    system(message);
    pthread_exit(NULL);
}


int main() {
  pid_t pid, sid;

  pid = fork();

  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();

  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  pthread_t thread[100];

  while(1) {
    
    // ngambil waktu sekarang
    time_t t=time(NULL);
	  struct tm tm = *localtime(&t);
    
    int menit = tm.tm_min;
    int jam = tm.tm_hour;
    int tanggal = tm.tm_mday;
    int bulan = tm.tm_mon + 1; 
    int hari = tm.tm_wday;

    FILE *fp;
  
    fp = fopen("/home/yasinta/crontab.data", "r"); // read mode
  
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
  
    char isi[1000];
    int i=0;
    int join;
    while (fgets(isi, 1000, fp) != NULL)  {
      char command_dijalankan[1000];
      if(check(tm,isi,command_dijalankan)){
        // printf("%s\n",command_dijalankan);
        int iret = pthread_create( &thread[i], NULL, print_message_function, (void*) command_dijalankan);
        i++;
      }
    }
    join = i;
    for(int p=0; p<join; p++)
    {
      pthread_join( thread[p], NULL);
    }
    fclose(fp);
    
  
    sleep(60-tm.tm_sec);
  }

  exit(EXIT_SUCCESS);
}
