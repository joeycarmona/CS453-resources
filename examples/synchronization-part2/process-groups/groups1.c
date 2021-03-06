#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
  pid_t pid1, pid2, pid3;
  int pipefd23[2];
  int pipefd12[2];
  int status;
  pid_t grp;

  grp = getpgid(getpid());
  printf("process id = %d, process group = %d \n",getpid(),grp);

  if ((pid1 = fork()) == 0) /* first child process */
  {
	 setpgid(0,0);
  	 grp = getpgrp();
  	 printf("process id = %d, process group = %d \n",getpid(),grp);
	 
     pipe (pipefd23);

     if ((pid2 = fork()) ==  0) { /* second child process */

  	    grp = getpgrp();
  	    printf("process id = %d, process group = %d \n",getpid(),grp);

     	pipe (pipefd12);
     	if ((pid3 = fork()) ==  0) { /* third child process */

  	     grp = getpgrp();
  	     printf("process id = %d, process group = %d \n",getpid(),grp);

         close(1);      /* close stdout  */
         dup(pipefd12[1]); /* points pipefd at file descriptor */
         close (pipefd12[0]);
         execlp ("ls", "ls", (char *)0);  /* child process does ls */

		} else { 
		 /* this is the middle process in the pipeline		*/
         close(1);      /* close stdout */
         dup(pipefd23[1]); /* points pipefd at file descriptor */
         close (pipefd23[0]);

         close(0); /* close stdin, normally fileno 0 */
         dup(pipefd12[0]);
         close (pipefd12[1]);

         execlp ("sort", "sort", (char *)0);  /* child process does ls */
		}

     } else {  

         close(0); /* close stdin, normally fileno 0 */
         dup(pipefd23[0]);
         close (pipefd23[1]);
         execlp ("wc", "wc", (char *)0); /* child does sort */
     }
   }
   /*parent waits for the last process in the pipeline to finish*/
   waitpid(pid1, &status, 0);
   exit(0);
}
