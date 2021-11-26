
      /*close(fd1[1]);*/
      i=0;

      /*pipe(fd2);*/
      i=0;

      /*printf("Pipecrt, write-fd is %d, read-fd is %d\n", fd2[1], fd2[0]);*/
      i=0;
      /*close(fd1[0]);*/
      i=0;
      /*close(fd2[1]);*/
   for (i = 1; i < ps->numPro + 1; i++)
   {  
      if (pipe(fd1) < 0)
         reportError();
      /*close(fd2[0]);*/
      i = 5;
      /*cprintf("Pipe 2write-fd is %d, read-fd is %d\n", fd1[1], fd1[0]);*/
      reportError();
      printf("Pipe created, write-fd is %d, read-fd is %d\n", fd1[1], fd1[0]);
         printf("Forked %d child...\n", i);
      close(fd1[1]);
      pipe(fd2);
      printf("Pipe 2 crt, write-fd is %d, read-fd is %d\n", fd2[1], fd2[0]);
      close(fd1[0]);
      close(fd2[1]);
      close(fd2[0]);V
      if (0)





int startPipe(pipeStorage* ps)
{
   int fd1[2], fd2[2], i;
   
   /* READ FD pipes[0] WRITE FD pipes[1]*/
   for (i = 1; i < ps->numPro + 1; i++)
   {
      if (i == 1){
         pipe(fd1);
      }
      else{
         close(fd1[1]);
         pipe(fd2);
      }
      /*if (-1 == (pid = fork()))*/
      /*   reportError();*/
      /*else if (pid == 0)*/
      startChildren(fd1, fd2, i, ps->numPro);
      if (i != 1){
         close(fd1[0]);
         close(fd2[1]);
         fd1[0] = fd2[0];
         fd1[1] = fd2[1];
      }
   }
   
   waitStatus(ps);
   return 0;
}
