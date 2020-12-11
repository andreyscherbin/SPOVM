#define MAX_COUNT 10

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <cstdlib>

using namespace std;
#define PID PROCESS_INFORMATION

#elif linux
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <cstdlib>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <iostream>

using namespace std;
#define _getch mygetchar
#define PID int
#define SEM_RESOURCE_MAX 1
#endif

class MYPROCESS
{
public:
    PID proc;
    int status;
    #ifdef _WIN32
    HANDLE SET_PRINT_EVENT,SET_NO_PRINT_EVENT;
    MYPROCESS(int count)
    {
        char name[50];
        char number[10];
        char eventprint[50]="print";
        char eventnoprint[50]="noprint";
        STARTUPINFO si;
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &proc, sizeof(proc) );
        wsprintf(name,"ChildProcess1 %d",count);
        wsprintf(number,"%d",count);
        strcat(eventprint,number);
        if(!CreateProcess(NULL, name ,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&proc))
            cout<<"FAIL"<<endl;
        SET_PRINT_EVENT = CreateEvent(NULL ,FALSE, FALSE,eventprint);
        strcat(eventnoprint,number);
        SET_NO_PRINT_EVENT = CreateEvent(NULL ,FALSE, FALSE,eventnoprint);
    }
    MYPROCESS(){};
#elif linux
    MYPROCESS(int count)
    {       
             sigset_t blockSet;
             sigemptyset(&blockSet);              //Start mask signal SIGUSR1
             sigaddset(&blockSet,SIGUSR1);
             sigprocmask(SIG_BLOCK,&blockSet,NULL);              
             char number[20];		 
             proc = fork ();                     
             if (proc == 0)
             {              
                sprintf(number, "%d", count);                
                execlp("./ChildProcess1","ChildProcess1",number,(char*)NULL);	             
                perror("exec");
             }
    }
#endif

    void SET_EVENTPRINT()
    {    
        #ifdef linux            
        kill(proc,SIGUSR1);       
        #elif _WIN32
        SetEvent(SET_PRINT_EVENT);
        #endif       
    }
	
    void SET_EVENTNOPRINT()
    {
        #ifdef linux
		kill(proc,SIGUSR2);       
        waitpid(proc,NULL,NULL);
        #elif _WIN32      
		SetEvent(SET_NO_PRINT_EVENT);
		WaitForSingleObject(proc.hProcess,INFINITE);	
		CloseHandle(proc.hThread); 
        CloseHandle(proc.hProcess);
		CloseHandle(SET_NO_PRINT_EVENT);
		CloseHandle(SET_PRINT_EVENT);
		       		
        #endif
    }    
};

#ifdef linux
int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;   
    if (!initialized)
	{       
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void set_keypress(termios & stored_settings)
    {
        struct termios new_settings;
        tcgetattr(0,&stored_settings);
        new_settings = stored_settings;
        /* для получения нажатых клавиш без потверждения enter-ом */
        new_settings.c_lflag &= (~ICANON); //перекл. в неканон. режим
        new_settings.c_lflag &= ~ECHO;     //не выводить нажатое
        new_settings.c_cc[VTIME] = 0;      //ждем
        new_settings.c_cc[VMIN] = 1;       //считать хоть 1 символ
        tcsetattr(0,TCSANOW,&new_settings);
    }
    void reset_keypress(termios & stored_settings)
    {
        tcsetattr(0,TCSANOW,&stored_settings);
    }
    char mygetchar()
    {
        struct termios stored_settings;
        set_keypress(stored_settings);
        char c = getchar();
        reset_keypress(stored_settings);
        return c;     
    }
#endif



