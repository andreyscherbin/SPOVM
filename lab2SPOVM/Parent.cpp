#include "ParentHeader.h"

MYPROCESS * ms[10];
int count=0;                                // process counter
int current=1;   
int status;

int mygetch()
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}

#ifdef _WIN32

int end(int count)
{
        char eventname[50]="completedprint";
        HANDLE printevent = OpenEvent(EVENT_ALL_ACCESS,FALSE,eventname);
      if(WaitForSingleObject(printevent,100)== WAIT_OBJECT_0)
        {
            if (!count) SetEvent(printevent);
			CloseHandle(printevent);
            return 1;
        }
    return 0;
}


#elif linux

int flagEnd = 1;
void setEndFlag(int signum)
{    
      flagEnd = 1;
}

void setendsignal()
{   
    struct sigaction endSignal;
    endSignal.sa_handler = setEndFlag;    
    sigaction(SIGUSR2,&endSignal,NULL);
}

union semun
{
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
}semopts;

struct sembuf sem_lock = { 0, -1, 0 };
struct sembuf sem_unlock = { 0, 1, 0 };
sigset_t blockSet, prevMask; 
key_t key;
int sem_id;

void CreateSemaphore()
{   
        sigemptyset(&blockSet);              //Start mask signal SIGUSR2
        sigaddset(&blockSet,SIGUSR2);       
        semopts.val = SEM_RESOURCE_MAX;
        if ((key = ftok("Parent", 's')) < 0)
	  	 {
             perror("ftok parent");			 
         }
         if((sem_id = semget(key, 1,IPC_CREAT|0666)) < 0)
         {   
			 perror("create semaphore : ");                   
         }
         if((semctl(sem_id, 0, SETVAL, semopts)) < 0)
		 {
             perror("semctl");
         }	
}

void WaitSemaphore()
{     
    if((sigprocmask(SIG_BLOCK,&blockSet,&prevMask)  == -1))
		{
      perror("sigprocmask1");
		}     
    if((semop(sem_id, &sem_lock, 1)) < 0 ) 
		{
     perror("waitParentEROR"); 
		} 
}
		
void ReleaseSemaphore()
{ 	
      if((semop(sem_id, &sem_unlock, 1)) < 0 )
	  {            
      perror("releaseParentERROR\n");                 
      }   
      if((sigprocmask(SIG_SETMASK,&prevMask,NULL)  == -1))
	  {
      perror("sigprocmask2");
	  }
} 
#endif

int main()
{   
#ifdef _WIN32  
    char completedprintfevent[50]="completedprint";
    HANDLE completedprintevent = CreateEvent(NULL ,FALSE,TRUE,completedprintfevent);
#elif linux      
         setendsignal();                //set new disposition  
	     CreateSemaphore(); 
#endif   
	
    while (true)
    { 
        char c = '+';

        while(c!='q')
        {   
            #ifdef linux         
            WaitSemaphore();
            #endif         
           
            c = mygetch(); 
			
			#ifdef linux
            ReleaseSemaphore();
			setendsignal();            
			#endif            
		
			switch(c)
         {
            case '+':
          if(count < MAX_COUNT)
          {          
            count++;
            MYPROCESS * process = new MYPROCESS(count);
            ms[count-1]=process;			   
          };
              break;
          case '-':
          if(count > 0)
          {           
            ms[count-1]->SET_EVENTNOPRINT();        
            count--;           
          };
              break;
         default:
          {
#ifdef _WIN32              
             if(end(count) && count > 0)
             {
                 if(current > count)
                     current = 1;
                 ms[current-1]->SET_EVENTPRINT();
                 current++;
             }
#elif linux               
              if(flagEnd && count > 0)
             {              
                 if(current > count)
                     current = 1;                      
                 ms[current-1]->SET_EVENTPRINT();
                 current++; 
                 flagEnd = 0;
             }               
#endif   
          }
          break;
        }
        }		
            for (int i=0;i<count;i++)
             {
                ms[i]->SET_EVENTNOPRINT();				
             }
            break;
    }	
    return 0;
    }



