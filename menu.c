#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#define NUMOPTIONS 8
struct OptionHeader{
    int OptionCount;
    char Title[64];
    struct Option *List;
};
struct Option{
    char OptionText[32];
};

int InputScan(void);
void PrintMenu(int select, struct OptionHeader *OptionHeader);
struct OptionHeader *InitializeOptions(FILE *OptionsFile);
int MainMenu(void);

void main(void){
    MainMenu();
}

int MainMenu(void){

    struct termios info;
    tcgetattr(0, &info);
    info.c_lflag &= ~ICANON;
    info.c_lflag &= ~ECHO;
    info.c_cc[VMIN] = 1;
    info.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &info);

    /*opens data file*/
    FILE *OptionsFile = fopen("options.txt","r");
    if(OptionsFile==NULL){
        printf("options file not found\n");
        return -1;
    }

    /*creates a table of the options from the file*/
    struct OptionHeader *OptionHeader;
    OptionHeader = InitializeOptions(OptionsFile);
    fclose(OptionsFile);

    /*input scan loop, gets the inputs and updates menu accordingly*/
    int SelectedOption = 0;
    int ioscan = 1;
    while(ioscan!=0){
        system("clear");
        PrintMenu(SelectedOption,OptionHeader);
        ioscan = InputScan();
        switch(ioscan){
        case 1:
            if(SelectedOption>0) SelectedOption--;
            break;
        case 2:
            if(SelectedOption<((OptionHeader->OptionCount)-2)) SelectedOption++;
            break;
        case 5:
            switch(SelectedOption){
            case 8:
                ioscan = 0;
                printf("goodbye!\n");
                info.c_lflag |= ICANON;
                info.c_lflag |= ECHO;
                tcsetattr(0, TCSANOW, &info);
                break;
            default:
                printf("option %d selected\n",SelectedOption+1);
                break;
            }
            sleep(1);
            break;
        }
    }
}
struct OptionHeader *InitializeOptions(FILE *OptionsFile){
    struct OptionHeader *OptionHeader;
    OptionHeader = (struct OptionHeader *)malloc(sizeof(struct OptionHeader));
    OptionHeader->OptionCount = 0;
    char text[32];
    int i;
    if(fgets(OptionHeader->Title,64,OptionsFile)==NULL){
        strcpy(OptionHeader->Title,"not found");
    }else{
        i = 0;
        while((OptionHeader->Title)[i]!='\n'){
            i++;
        }
        (OptionHeader->Title)[i] = '\0';
    }
    fgets(text,32,OptionsFile);
    if(text!=NULL){
        struct Option *Option;
        Option = (struct Option *)malloc(sizeof(struct Option));
        strcpy(Option->OptionText,text);
        i = 0;
        while((Option->OptionText)[i]!='\n'){
            i++;
        }
        (Option->OptionText)[i] = '\0';             
        OptionHeader->OptionCount++;
        OptionHeader->List = Option;
        while(fgets(text,32,OptionsFile)!=NULL){
            Option = (struct Option *)realloc((void *)Option,sizeof(struct Option) * (OptionHeader->OptionCount+1));
            strcpy((Option+(OptionHeader->OptionCount))->OptionText,text);
            i = 0;
            while(((Option+(OptionHeader->OptionCount))->OptionText)[i]!='\n'){
                i++;
            }
            ((Option+(OptionHeader->OptionCount))->OptionText)[i] = '\0';            
            OptionHeader->OptionCount++;
        }
    }
    return OptionHeader;
}

int InputScan(void){;
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
            /*esc*/
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

void PrintMenu(int select, struct OptionHeader *OptionHeader){ 
    printf("\n<<%s>>\n",OptionHeader->Title);
    int i;
    printf("\noptions\n");
    printf("=======\n");
    for(i=0;i<(OptionHeader->OptionCount)-1;i++){
        printf("%d)",i+1);
        if(i==select){
            putc('[',stdout);
        }else{
            putc(' ',stdout);
        }
        printf("%s",((OptionHeader->List)+i)->OptionText);
        if(i==select){
            putc(']',stdout);
        }else{
            putc(' ',stdout);
        }
        putc('\n',stdout);      
    }
}