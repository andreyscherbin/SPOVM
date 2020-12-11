#ifdef _WIN32
using namespace std;
#include <iostream>
#include <conio.h>
#include <windows.h>

#elif linux
#include <stdio.h>
#include <cstring>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <errno.h>
#include <sys/ioctl.h>

using namespace std;
char mygetch();
#endif




#ifdef _WIN32

int number;

int check_event()
{      
       
	   
        HANDLE exitprocess;
        char EXITPROCESS[10]="exit";       		
		char numberstring[10];
		wsprintf(numberstring,"%d",number);
		strcat(EXITPROCESS,numberstring);
        fflush(stdout);    
        exitprocess = OpenEvent(EVENT_ALL_ACCESS,FALSE,EXITPROCESS);
        if(WaitForSingleObject(exitprocess,10) == WAIT_OBJECT_0)
        {          
			
	    CloseHandle(exitprocess);
            return 1;
        }       
        return 0;    
}


int main(int argc, char* argv[])
{
	number  =atoi(argv[3]);	 
	char format[50] = ".java .asm .cpp";
	char DangerousFormat[50] = ".txt";
	char programmerString[50];
	HANDLE write, read;
	DWORD dw;
	if (argc > 1)
	{
		write = (HANDLE)atoi(argv[1]);
		read = (HANDLE)atoi(argv[2]);		
		ReadFile(read, programmerString,50, &dw, NULL);			
	}

	if (argc > 1 && strstr(format, programmerString))
	{
		char succes[50] = "Compiled";
		cout << succes;
		WriteFile(write, succes,50, &dw, NULL);		
	}
	
	if (argc > 1 && strstr(DangerousFormat,programmerString))
	{
		char DangerousString[50] = "Dangerous format";
		cout << DangerousString;
		WriteFile(write,DangerousString,50, &dw, NULL);			
	}

	if (argc > 1 && !strstr(format, programmerString))
	{
		char fail[50] = "Do not support";	
		cout << fail;
		WriteFile(write,fail,50, &dw, NULL);		
	}
	printf("\nEnter any key to exit\n");
        while(true)
{
	if(check_event() == 1)
	{
		CloseHandle(read);
	    CloseHandle(write);
        return 1;
	}
    if (_kbhit())
	{  
        CloseHandle(read);
	    CloseHandle(write);
       return 1;
	}
}	
}


#elif linux
int _kbhit();
int flag = 1;
struct sigaction stopSignal;
void stopCompiler(int num)
{     
    flag = 0;  
  
}


int main(int argc, char* argv[])
{
	 stopSignal.sa_handler = stopCompiler;
     sigaction(SIGUSR2,&stopSignal,NULL);	
	
    char format[50] = ".java .asm .cpp";
    char DangerousFormat[50] = ".txt";
    char programmerString[50];
    char anyKey[50] = "Press any key to exit";
    int writel,readl;

    if(argc>1)
	{
    readl = atoi(argv[1]);
    writel = atoi(argv[2]);
    read(readl,programmerString,50);
    }

    if (argc > 1 && strstr(format, programmerString))
	{
        char succes[50] = "Compiled\n";
        cout << succes;      
        write(writel, succes,50);
		cout << "\n" << programmerString << endl;       
    }
	
    if (argc > 1 && strstr(DangerousFormat,programmerString))
	{
        char DangerousString[50] = "Dangerous format\n";
        cout << DangerousString;       
        write(writel,DangerousString,50);
		cout << "\n" << programmerString << endl;        
    }

    if (argc > 1 && !strstr(format, programmerString))
	{
        char fail[50] = "Do not support\n";
        cout << fail;       
        write(writel,fail,50);
		cout << "\n" << programmerString << endl;       
    }
	printf("Enter any key to exit\n");
	while(true)
	{
		if(!flag)
		{
			close(writel);
		    close(readl);
			break;
		}
		if(_kbhit())
			break;		
	}
		
		
}

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
new_settings.c_lflag &= (~ICANON);
new_settings.c_lflag &= ~ECHO;
new_settings.c_cc[VTIME] = 0;
new_settings.c_cc[VMIN] = 1;
tcsetattr(0,TCSANOW,&new_settings);
}

char mygetch()
{
struct termios stored_settings;
set_keypress(stored_settings);
return getchar();
}
#endif



