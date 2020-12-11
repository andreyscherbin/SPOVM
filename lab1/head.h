#ifdef _WIN32
using namespace std;
#include <iostream>
#include <conio.h>
#include <windows.h>
#define PID PROCESS_INFORMATION

#elif linux
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <ncurses.h>

#define PID int
#define EDOM
#define EILSEQ
#define ERANGE
extern int errno;
using namespace std;
#endif

class MYPROCESS
{
public:
	PID proc;
	
#ifdef _WIN32

	MYPROCESS()
	{   
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

        wsprintf(wayChild, "Compiler %s %s", stringWrite, stringRead);
		
		while (!strstr(format, DangerousString)) {

			cout << "choose format" << " .java .asm .cpp\n";
			cin >> format;
			WriteFile(write, format, 50, &dw, NULL);
			CreateProcess(NULL, wayChild, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &proc);

			SYSTEMTIME lt;
			while (WaitForSingleObject(proc.hProcess, 50)) {
				GetLocalTime(&lt);
				printf("%02d:%02d:%02d \r", lt.wHour, lt.wMinute, lt.wSecond);
			}			
			ReadFile(read, format, 50, &dw, NULL);
			cout << '\n';
			CloseHandle(proc.hProcess);
			CloseHandle(proc.hThread);
		}
		getch();	
		
		CloseHandle(read);
		CloseHandle(write);
	}
#elif linux
	MYPROCESS()
	{
        initscr();
        curs_set(0);
        time_t ltime;
        char stringWrite[50];
        char stringRead[50];
        char format[50];
        char DangerousString[50] = "Dangerous format";       
	int pipedes[2];
        int st;
        int i = -1;
        pid_t pid;

        pipe(pipedes);

        snprintf( stringWrite,sizeof(stringRead),"%d",pipedes[1]);
        snprintf( stringRead,sizeof(stringRead),"%d",pipedes[0]);

        while (!strstr(format, DangerousString)) {

        printw("\nchoose format .java .asm .cpp\n");
        getstr(format);
        write(pipedes[1], format, 50);

        pid = fork();
	if (pid > 0)
        {
             i = i+4;
            while(1) {
               refresh();
               time(&ltime);
               move(i,0);
               printw(ctime(&ltime));
            if(waitpid(pid,&st,WNOHANG) > 0)
                break;
             napms(50);
            }
            read(pipedes[0],format,50);
            addstr(format);

		}
		if (pid == 0)
        {           
            execlp("/usr/bin/xterm","xterm","-e","./ChildProcess",stringRead,stringWrite,(char*)NULL);
        }
        }
        endwin();
        close(pipedes[0]);
        close(pipedes[1]);
	}
#endif
};



