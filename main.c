#include <stdio.h>
#include <string.h>
#include "mp3.h"

Data data;

/* Tag names in ID3v2 format (TPE1, TIT2, ...) */
char * tag[] = {"TPE1","TIT2","TALB","TCON","COMM","TYER"};

/* Corresponding command-line symbols (-a, -t, ...) */
char * symbol[] = {"-a","-t","-A","-c","-m","-y"};

int main(int argc , char * argv[])
{
    char Type = SUCCESS;

    /*--------------------------------------------------------------
     * If user requests help: ./a.out --help
     *--------------------------------------------------------------*/
    if(argv[1] != NULL && !strcmp(argv[1], "--help"))
    {
        Help(argv[0]);
        return FAILURE;
    }

    /*--------------------------------------------------------------
     * Check operation type (-v / -e)
     *--------------------------------------------------------------*/
    if(argv[1] != NULL)
        Type = Check_operation_Type(1 , argv);
    else
    {
        /* Error: No valid arguments */
        printf(T"INFO  :"E" "R "## ERROR ##"E"\n");
        printf(T"USAGE :"E" \n");
        printf("\t"O"To View Pass     :"E" %s -v mp3file\n" , argv[0]);
        printf("\t"O"To Edit Pass     :"E" %s -e -[type] replace_text mp3file\n" , argv[0]);
        printf("\t"O"To Get Help Pass :"E" %s --help\n\n" , argv[0]);
        return FAILURE;
    }

    /*--------------------------------------------------------------
     * If -e is passed, validate the second symbol (like -t, -a)
     *--------------------------------------------------------------*/
    if(Type == EDIT && Check_operation_Type(2 , argv) == FAILURE)
    {
        printf(T"INFO  :"E" "R "## ERROR ##"E"\n");
        printf("Given Tag Symbol "B"< %s >"E" is Not Valid\n" , argv[2]);
        printf(T"Usage :"E" %s --help\n" , argv[0]);
        return FAILURE;
    }

    /*--------------------------------------------------------------
     * Check File Extension must be .mp3
     *--------------------------------------------------------------*/
    if((Type == VIEW && strcmp(strrchr(argv[2] , '.'), ".mp3")) ||
       (Type == EDIT && strcmp(strrchr(argv[4] , '.'), ".mp3")))
    {
        printf(T"INFO :"E" "R "# ERROR #\n"E);
        printf("Given File is Not "B"< .mp3 >"E" Extension\n");
        return FAILURE;
    }

    /* Number of supported tag types */
    unsigned char Length = sizeof(tag) / sizeof(tag[0]);

    /*--------------------------------------------------------------
     * VIEW MODE PROCESSING
     *--------------------------------------------------------------*/
    if(Type == VIEW)
    {
        SongInfo viewinfo;
        viewinfo.Tag_size = Length;

        if(Open_File(VIEW , argv[2] , &viewinfo) == FAILURE)
            return FAILURE;

        View_Details_Mp3(&viewinfo);
    }

    /*--------------------------------------------------------------
     * EDIT MODE PROCESSING
     *--------------------------------------------------------------*/
    else
    {
        SongInfo editinfo;
        editinfo.Tag_size = Length;

        if(Open_File(EDIT , argv[4] , &editinfo) == FAILURE)
            return FAILURE;

        /* Edit tag and update MP3 */
        if(Edit_Details_Mp3(&editinfo , argv[3] , argv[2]) == SUCCESS)
        {
            remove(editinfo.songname);                         /* Remove old file       */
            rename(editinfo.Tempsongname , editinfo.songname); /* Rename edited temp    */
        }
        else
            remove(editinfo.Tempsongname);                     /* Remove temp on error */
    }
}

/*------------------------------------------------------------------------------
 * Function: Check_operation_Type
 * Purpose : Map command-line flag to corresponding ID3 operation or frame type.
 *----------------------------------------------------------------------------*/
outcome Check_operation_Type(char pos , char * argv[])
{
    if(argv[pos][1] == 'v')
        return VIEW;

    else if(argv[pos][1] == 'e')
        return EDIT;

    else if(argv[pos][1] == 't')
        return TITTLE;

    else if(argv[pos][1] == 'a')
        return ARTIST;

    else if(argv[pos][1] == 'A')
        return ALBUM;

    else if(argv[pos][1] == 'y')
        return YEAR;

    else if(argv[pos][1] == 'm')
        return CONTENT;

    else if(argv[pos][1] == 'c')
        return COMMENT;

    else
        return FAILURE;
}

