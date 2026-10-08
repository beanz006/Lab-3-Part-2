#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void ClientProcess(volatile int SharedMem[]);

int  main(void)
{
     int ShmID;
     volatile int *ShmPTR;
     pid_t pid;
     int status;
     int account, balance;

     ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
      perror("shmget error");
      exit(1);
     }

     ShmPTR = (volatile int *)shmat(ShmID, NULL, 0);

     if (ShmPTR == (void *)-1) {
      perror("shmat error");
      shmctl(ShmID, IPC_RMID, NULL);
      exit(1);
     }
     ShmPTR[0] = 0;
     ShmPTR[1] = 0;
     pid = fork();

     if (pid < 0) {
          perror("fork error");
          shmdt((const void *)ShmPTR);
          shmctl(ShmID, IPC_RMID, NULL);
          exit(1);
     }
     if (pid == 0) {
          ClientProcess(ShmPTR);
          shmdt((const void *)ShmPTR);
          exit(0);
     }
     else {
      srand(time(NULL)^ getpid());
      
      for (int i = 0; i < 25; i++){
        sleep(rand() % 6);

        while (ShmPTR[1]!=0){

        }
        account = ShmPTR[0];

        if(account <= 100){
          balance = rand() % 101;

          if (balance % 2 == 0){
            account += balance;
            printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
          }
          else{
            printf("Dear old Dad: Doesn't have any money to give\n");
          }
        }
        else {
          printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }
        ShmPTR[0] = account;
        ShmPTR[1] = 1;
      }
      wait(&status);
      shmdt((const void *)ShmPTR);
      shmctl(ShmID, IPC_RMID, NULL);
    }
    return 0;
}
void ClientProcess(volatile int SharedMem[]){
  int account, balance;

  srand(time(NULL) ^ getpid());

  for (int i = 0; i < 25; i++){
    sleep(rand() % 6);

    while(SharedMem[1]!= 1){

    }
    account = SharedMem[0];
    balance = rand() % 51;
    printf("Poor Student needs $%d\n", balance);
    
    if (balance <= account){
      account -= balance;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
    }
    else{
      printf("Poor Student: Not Enough Cash ($%d)\n", account);
    }
    SharedMem[0] = account;
    SharedMem[1] = 0;
  }
}
