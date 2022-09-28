#include <stdlib.h>
#include <stdio.h>
#ifdef __unix
#include <sys/ioctl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#endif

void clear_screen()
{
#ifdef __WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
#else
    printf("\033[J");
#endif
}

void move_cursor(int x, int y)
{
#ifdef __WIN32
    COORD coordScreen;
    coordScreen.X = x;
    coordScreen.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordScreen );
#else
    printf("\033[%d;%dH", y, x);
#endif
}

void get_terminal_size(int *w, int *h)
{
#ifdef __unix
    struct winsize ws;
    ioctl(1, TIOCGWINSZ, &ws);
    *h = ws.ws_row;
    *w = ws.ws_col;
#else
#ifdef __WIN32
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);
    *h = consoleScreenBufferInfo.dwSize.Y;
    *w = consoleScreenBufferInfo.dwSize.X;
#else
    *h = 25;
    *w = 80;
#endif
#endif
}

struct cmd_line_params
{
    int bits;
    int channels;
};

void parse_cmd_line(int argc, char *argv[], struct cmd_line_params *params)
{
    if(argc>=2)
    {
        params->bits = atoi(argv[1]);
    }
    else
    {
        params->bits = 16;
    }
    if(argc>=3)
    {
        params->channels = atoi(argv[1]);
    }
    else
    {
        params->channels = 2;
    }
}

int main(int argc, char *argv[])
{
    struct cmd_line_params params;
    parse_cmd_line(argc, argv, &params);
    int w, h;
    get_terminal_size(&w, &h);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    int k;
    int x;
    for(k = 0; ; k = (k+1) % 50)
    {
        int oldy;
        for(x = 0; x < w*1; x = x++)
        {
            if(k==0)
            {
                if(x==0)
                {
                    move_cursor(0, 0);
                    clear_screen();
                }
            }
            long long sample = 0;
            int i, j;
            for(i = 0; i < params.channels * 1; i++)
            {
                long long chsample = 0;
                for(j = 0; j < params.bits/8; j++)
                {
                    int c = getchar();
                    if(c == EOF)
                    {
                        return 0;
                    }
                    chsample <<= 8;
                    chsample += c;
                }
                chsample = (chsample ^ (1ULL<<(params.bits-1))) - (1ULL<<(params.bits-1)); //sign extension
                sample = (sample + chsample)/2;
            }
            if(k==0)
            {
                int l;
                int y = h/2 - (((double)h / (1LL << params.bits)) * sample);
                if(x==0)
                {
                    oldy = y;
                }
                for(l = oldy; y>oldy? l<=y : l>=y; y>oldy ? l++ : l--)
                {
                    move_cursor(x/1, l);
                    putchar('+');
                }
                oldy = y;
            }
        }
    }
    return 0;
}
