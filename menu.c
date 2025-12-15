#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#define NUMOPTIONS 8
int InputScan(struct termios *terminal);
void PrintMenu(int select);

void main(void){
    struct termios info;
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
    info.c_cc[VTIME] = 0;         /* no timeout */
    info.c_lflag &= ~ECHO;      /* disable echo*/
    tcsetattr(0, TCSANOW, &info); /* set immediately */

    //FILE *fp = freopen("output.txt", "w", stdin);
    int select = 0;
    int ioscan;
    while(1){
        system("clear");
        PrintMenu(select);
        ioscan = InputScan(&info);
        if(ioscan==1&&select>0){
            select--;
        }else if(ioscan==2&&select<(NUMOPTIONS-1)){
            select++;
        }else if(ioscan==5){
            printf("option %d selected\n",select+1);
            sleep(2);
            int c;
        }
    }

}

int InputScan(struct termios *terminal){;
    
    /***input queue deletion***/
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN; 
    char c[80];
    int flag = 0;
    int ret;
    while(flag==0){
        int ret = poll(&pfd, 1, 0);
        if (ret > 0 && (pfd.revents & POLLIN)){
            read(STDIN_FILENO, c, 80);
        }else{
            flag = 1;
        }
    }
    /***input queue deletion***/

    int returnvalue = -1;
    char userinput = 0;
    while(returnvalue==-1){ 
        if(userinput==10){
            /*enter*/
            returnvalue = 5;
        }else if(userinput==27){
            userinput = getchar();
            if(userinput==91){
                returnvalue = -1;
                userinput = getchar();
                switch(userinput){
                case 65:
                    /*up*/
                    returnvalue = 1;
                    break;
                case 66:
                    /*down*/
                    returnvalue = 2;
                    break;            
                }
            }
        }else{
            userinput = getchar();
        }
    }
    return returnvalue;
}

void PrintMenu(int select){
        
    char menustring[] = "Fabionno's Brazzianos, e.t. GRIGORI";
    printf("%s\n",menustring);
    int i;
    printf("\noptions\n");
    for(i=0;i<sizeof(menustring);i++){
        putc('=',stdout);
    }
    char options[NUMOPTIONS][32];
    strcpy(options[0],"spaghetti and meatballs - $100");
    strcpy(options[1],"spaghetti and meatsauce - $5");
    strcpy(options[2],"house salad - $50");
    strcpy(options[3],"chicken alfredo - $10");
    strcpy(options[4],"lasagna - $20");
    strcpy(options[5],"garlic bread - no charge");
    strcpy(options[6],"carne de asada - $30");
    strcpy(options[7],"water cup - $3");

    putc('\n',stdout);
    for(i=0;i<NUMOPTIONS;i++){
        printf("%d)",i+1);
        if(i==select){
            putc('[',stdout);
        }else{
            putc(' ',stdout);
        }
        printf("%s",options[i]);
        if(i==select){
            putc(']',stdout);
        }else{
            putc(' ',stdout);
        }
        putc('\n',stdout);      
    }

}