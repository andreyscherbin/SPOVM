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
using namespace std;
#endif

#ifdef _WIN32
int main(int argc, char* argv[])
{
	char format[50] = ".java .asm .cpp";
	char DangerousFormat[50] = ".txt";
	char programmerString[50];

	HANDLE write, read;
	DWORD dw;
	if (argc > 1) {
		write = (HANDLE)atoi(argv[1]);
		read = (HANDLE)atoi(argv[2]);		
		ReadFile(read, programmerString,50, &dw, NULL);			
	}

	if (argc > 1 && strstr(format, programmerString)) {
		char succes[50] = "Compiled";
		cout << succes;
		WriteFile(write, succes,50, &dw, NULL);		
		
	}
	if (argc > 1 && strstr(DangerousFormat,programmerString)) {
		char DangerousString[50] = "Dangerous format";
		cout << DangerousString;
		WriteFile(write,DangerousString,50, &dw, NULL);	
		
	}

	if (argc > 1 && !strstr(format, programmerString)) {
		char fail[50] = "Do not support";	
		cout << fail;
		WriteFile(write,fail,50, &dw, NULL);		
		
	}
	getch();
	CloseHandle(read);
	CloseHandle(write);

}
#elif linux



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
void reset_keypress(termios & stored_settings)
{
tcsetattr(0,TCSANOW,&stored_settings);
}
char mygetch()
{
struct termios stored_settings;
set_keypress(stored_settings);
return getchar();
}




int main(int argc, char* argv[])
{
    char format[50] = ".java .asm .cpp";
    char DangerousFormat[50] = ".txt";
    char programmerString[50];
    char anyKey[50] = "Press any key to exit";
    int writel,readl;

    if(argc>1) {

    readl = atoi(argv[1]);
    writel = atoi(argv[2]);
    read(readl,programmerString,50);
    }

    if (argc > 1 && strstr(format, programmerString)) {
        char succes[50] = "Compiled\n";
        cout << succes;
        cout << anyKey;
        write(writel, succes,50);
        mygetch();
        close(writel);
        close(readl);
        return 0;

    }
    if (argc > 1 && strstr(DangerousFormat,programmerString)) {
        char DangerousString[50] = "Dangerous format\n";
        cout << DangerousString;
        cout << anyKey;
        write(writel,DangerousString,50);
        mygetch();
        close(writel);
        close(readl);
        return 0;
    }

    if (argc > 1 && !strstr(format, programmerString)) {
        char fail[50] = "Do not support\n";
        cout << fail;
        cout << anyKey;
        write(writel,fail,50);
        mygetch();
        close(writel);
        close(readl);
        return 0;
    }   
}
#endif



