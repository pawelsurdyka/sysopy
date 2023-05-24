#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <string.h>

void sig_info_handler (int sig, siginfo_t * info, void *ucontext)
{
  printf ("SA_SIGINFO\n");
  printf ("Signal number %d\n", info->si_signo);
  printf ("Signal has been sent by process %d\n", info->si_pid);
  printf ("STATUS: %d\n",info->si_status);
  printf ("System time consumed: %ld\n", info->si_stime);
  printf ("\n");
}

void test_sig_info (void)
{
  struct sigaction action;
  action.sa_sigaction = sig_info_handler;
  action.sa_flags = SA_SIGINFO;
  sigemptyset (&action.sa_mask);

  if (sigaction (SIGUSR1, &action, NULL) == -1)
    {
      printf ("Error when assigning action to signal\n");
      return;
    }
  kill (getpid (), SIGUSR1);
}

void resethand_handler (int sig)
{
  printf ("Signal number: %d, PID: %d, PPID: %d\n", sig, getpid (),getppid ());
}

void test_sa_resethand ()
{
  printf ("SA_RESETHAND\n");
  static struct sigaction sig;
  sig.sa_handler = resethand_handler;
  sigemptyset (&sig.sa_mask);
  if (sigaction (SIGCHLD, &sig, NULL) == -1)
  {
    perror ("Sigaction error\n");
  }
  printf ("before\n");

  pid_t pid;
  if ((pid = fork ()) == 0)
    {
      while (1);
    }
  else
    {
      sleep (1);
      printf ("%d sending %d to %d\n", getppid (), SIGSTOP, pid);
      kill (pid, SIGSTOP);
      sleep (1);
    }

  printf ("after\n");
  
  sig.sa_flags = SA_RESETHAND;
  if ((pid = fork ()) == 0)
    {
      while (1);
    }
  else
    {
      sleep (1);
      printf ("%d sending %d to %d\n", getppid (), SIGSTOP, pid);
      kill (pid, SIGSTOP);
      sleep (1);
    }
}

void nocldstop_handler(int sig) {
    printf("I got signal, PID: %d, PPID: %d\n", getpid(), getppid());
}


void test_sa_nocldstop() {
    printf("\nSA_NOCLDSTOP\n");
    struct sigaction sig;
    sig.sa_handler = &nocldstop_handler;
    sig.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sig, NULL);
    int pid = fork();
    if(pid == 0) {
        while(1);
    } else {
        kill(pid, SIGSTOP);
        sleep(1);
        kill(pid, SIGCONT);
        kill(pid, SIGKILL);
    }
    printf("before signal kill\n");
    wait(NULL);
    printf("after signal kill\n");

}


int main ()
{

  test_sig_info ();
  test_sa_resethand ();
  test_sa_nocldstop();

  return 0;
}
