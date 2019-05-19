
char STRING[10][30] = {{"1) First programmer"}, {"2) Second programmer"}, {"3) Third programmer"}, {"4) Fourth programmer"}, {"5) Fifth programmer"},{"6) Sixth programmer"}, {"7) Seventh programmer"}, {"8) Eighth programmer"}, {"9) Ninth programmer"}, {"10) Tenth programmer"}};

#ifdef _WIN32
using namespace std;
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#define PID PROCESS_INFORMATION

int check_event(char * number);
void PRINTCOMPLETED();

int check_event(char * number)
{
    while(1)
    {
		
		
        char noprint[50]="noprint";
        char print[50]="print";
        strcat(noprint,number);
        strcat(print,number);
        fflush(stdout);
        HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,noprint);
        if(WaitForSingleObject(printevent,10) == WAIT_OBJECT_0)
        {   
	        CloseHandle(printevent);
            return -1;
        }

        printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,print);
        if(WaitForSingleObject(printevent,10) == WAIT_OBJECT_0)
        {    
	        CloseHandle(printevent);
            return 1;
        }       
        return 0;
    }
}
void PRINTCOMPLETED()
{
        char completed[50]="completedprint";
        HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,completed);
        SetEvent(printevent);
		CloseHandle(printevent);
}

#elif linux
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#define PID int
#define SEM_RESOURCE_MAX 1

union semun
{
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
}semopts;

struct sigaction printSignal,stopSignal;
int printFlag = 0;
struct sembuf sem_lock = { 0, -1, 0 };
struct sembuf sem_unlock = { 0, 1, 0 };
sigset_t blockSet, prevMask;
key_t key;
int sem_id;

void canPrint(int num)
{   
    printFlag = 1;    
}

void stopPrint(int num)
{     
    printFlag = -1;  
  
}
void PRINTCOMPLETED()
{   
    kill(getppid(),SIGUSR2);
}

void CreateSemaphore()
{
        sigemptyset(&blockSet);              //Start mask signal SIGUS1
        sigaddset(&blockSet,SIGUSR1); 
	
  if( (key = ftok ("Parent",'s')) < 0)
         {
             perror("no open key");
         }
         if( ( sem_id = semget(key,1,IPC_CREAT|0666)) < 0)
		 {
              perror("no open semaphore");
         }  
}

void WaitSemaphore()
{	
 if((sigprocmask(SIG_BLOCK,&blockSet,&prevMask)  == -1))
      perror("sigprocmask1");
	
   if((semop(sem_id, &sem_lock, 1)) < 0 )
   {
      perror("waitChildERROR\n");              
   }	
}

void ReleaseSemaphore()
{     
  if((semop(sem_id, &sem_unlock, 1)) < 0 )
  {
     perror("releaseSemaphore child");            
  }

 if((sigprocmask(SIG_SETMASK,&prevMask,NULL)  == -1))
 {
     perror("sigprocmask2");        
 }
}  
#endif
	
#ifdef _WIN32
int main(int argc, char *argv[])
	{ 
                    
		int number;
		char exit[10]="exit";
        number=atoi(argv[1]);		
		char numberstring[10];
		wsprintf(numberstring,"%d",number);
		strcat(exit,numberstring);
		HANDLE exitprocess;
        exitprocess = CreateEvent(NULL ,FALSE, FALSE,exit);  
        PID proc;  
		char wayChild[100];
		char stringWrite[50];
		char stringRead[50];
		char format[50];
		char DangerousString[50] = "Dangerous format";	
		STARTUPINFO si;
		HANDLE write, read;
		DWORD dw;		
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		CreatePipe(&read, &write, &sa, 0);
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&proc, sizeof(proc));
		si.cb = sizeof(STARTUPINFO);		
		itoa((int)write, stringWrite, 10);
		itoa((int)read, stringRead, 10);
        wsprintf(wayChild, "Compiler %s %s %s", stringWrite, stringRead,numberstring);	
		cout << "\nProgrammer " << number;
		cout << " choose format" << " .java .asm .cpp\n";
		cin >> format;
		WriteFile(write, format, 50, &dw, NULL);
		CreateProcess(NULL, wayChild, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &proc);		
		while (true)
        {
        int printFlag = check_event(argv[1]);      
        if ( printFlag ==1)
         {
            for (int i = 0;STRING[number-1][i]!=0;i++)
            {
                printf("%c",STRING[number-1][i]);
                fflush(stdout);
                Sleep(10);
            }
            putchar('\n');
            fflush(stdout);
            printFlag=0;
            PRINTCOMPLETED();
         }
        if (printFlag== -1)
         {
             SetEvent(exitprocess);			
             WaitForSingleObject(proc.hProcess,INFINITE);           
             CloseHandle(proc.hThread);			 
		     CloseHandle(proc.hProcess);		   
             CloseHandle(read);
		     CloseHandle(write);            
             return 1;
         } 
        }			
	}
#elif linux

int main(int argc, char *argv[])
	{	  
       printSignal.sa_handler = canPrint;
       sigaction(SIGUSR1,&printSignal,NULL);          // set new dispos SIGUSR1
       stopSignal.sa_handler = stopPrint;
       sigaction(SIGUSR2,&stopSignal,NULL);           // set new dispos SIGUSR2
          
       sigset_t unblockSet;
       sigemptyset(&unblockSet);                      //unmask signal SIGUSR1;
       sigaddset(&unblockSet,SIGUSR1);
       sigprocmask(SIG_UNBLOCK,&unblockSet,NULL);         
     
       int number=1;
       number=atoi(argv[1]);
       PID proc;       
       char stringWrite[50];
       char stringRead[50];
       char format[50]=".java";
       char DangerousString[50] = "Dangerous format";       
       int pipedes[2];      
       pid_t pid;
       pipe(pipedes);
       snprintf( stringWrite,sizeof(stringRead),"%d",pipedes[1]);
       snprintf( stringRead,sizeof(stringRead),"%d",pipedes[0]); 
	
       CreateSemaphore();              
       WaitSemaphore();     
       printf("\nProgrammer %d choose format .java .asm .cpp\n",number);        
       scanf("%s",format);   
       ReleaseSemaphore(); 
       write(pipedes[1], format, 50);
       pid = fork(); 		  
      if ( pid > 0)
      {		  
while (true)
    {       
       if( waitpid(pid,0,WNOHANG) > 0)
       {        
         read(pipedes[0],format,50);                    
         WaitSemaphore();   
         printf("\nProgrammer %d choose format .java .asm .cpp\n",number);            
         scanf("%s",format); 
         ReleaseSemaphore(); 
         write(pipedes[1], format, 50);         
         pid = fork();
         
         if(pid == 0)
         {            
            execlp("/usr/bin/xterm","xterm","-e","./Compiler",stringRead,stringWrite,(char*)NULL);
         }
       }       	  
	  
        if ( printFlag ==1)
        {                                       
            sigemptyset(&blockSet);              //Start mask signal SIGUSR2 ( printFlag = -1)
            sigaddset(&blockSet,SIGUSR2); 
            if((sigprocmask(SIG_BLOCK,&blockSet,&prevMask)  == -1))
			{
                perror("sigprocmask1");
			}
            
            for (int i = 0;STRING[number-1][i]!=0;i++)
            {			
                printf("%c",STRING[number-1][i]);				
               	fflush(stdout);
                usleep(10000);
            }
            printf("%c",'\n');	                     		
            printFlag=0;
            PRINTCOMPLETED();
            
             if((sigprocmask(SIG_SETMASK,&prevMask,NULL)  == -1))  // End mask signal SIGUSR2 ( printFlag = -1)
			 {
                perror("sigprocmask2");
			 }
        }
        if (printFlag== -1)
        {             
            kill(pid,SIGUSR2);
            waitpid(pid,NULL,NULL);
            break;          
        }
        }	        
      }
    if ( pid == 0)
    {
      execlp("/usr/bin/xterm","xterm","-e","./Compiler",stringRead,stringWrite,(char*)NULL);
    }
        close(pipedes[0]);
        close(pipedes[1]);  
        return 0;
}
#endif


	
	
























