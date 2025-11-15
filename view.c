#include <stdio.h>
#include <string.h>
#include "mp3.h"


char * Info[] = {"Artist","Tittle","Album","Content","Comment","Year"};  // Info[] → Human-readable label for each tag (Artist, Title, Album, etc.)
extern char * tag[];                                                     // tag[]  → ID3 frame identifiers like "TPE1", "TIT2", ...
extern char * symbol[];                                                  // symbol[] → User-friendly codes mapped to tag[]


outcome View_Details_Mp3(SongInfo * viewinfo)
{
    fseek(viewinfo->fptr_songname , 10 , SEEK_CUR);                     // Skip ID3 header (10 bytes) since fptr_songname is at beginning

    printf(T"\n|----------------------------------------!"B" View Mode of MP3 "E"! "T"----------------------------------------|\n\n"E);
    printf("MP3 FILE "B"< %s >"E" TAG SIZE FROM HEADER (syncsafe) :"O" %ld Bytes\n"E,
            viewinfo->songname , viewinfo->FrameSize);
    printf(T"______________________________________________________________________________________________________\n\n"E);

    char Tag[4];

    while(viewinfo->FrameSize + 10 > ftell(viewinfo->fptr_songname))     // Loop until we reach the end of tag frames (FrameSize + header size)
    {
        fread(Tag , 4 , 1 , viewinfo->fptr_songname);                    // Read 4-byte frame identifier (ex: "TPE1", "TIT2")

        if(Tag[0] == 0 || !strncmp(Tag ,"APIC",4))                       // Stop if padding or APIC frame encountered
            break;

        fread(&data.size , 4 , 1 , viewinfo->fptr_songname);             // Read 4-byte frame size (syncsafe)

        Converstion(data.arr);                                           // Convert size from syncsafe to normal integer

        char DATA[data.size];                                            // Allocate buffer to hold frame data
        fseek(viewinfo->fptr_songname , 3 , SEEK_CUR);                   // Skip encoding byte + language/description (commonly 3 bytes)

        fread(DATA , data.size - 1 , 1 , viewinfo->fptr_songname);       // Read actual text data (excluding 1 unused padding byte)

        for(char i = 0 ; i < viewinfo->Tag_size ; i++)                   // Match this tag against known tag list
        {
            if(!strncmp(Tag , tag[i] , 4))
            {
                printf("%-20s : ", Info[i]);

                for(int i = 0 ; i < data.size - 1 ; i++)
                {
                    if(DATA[i] != 0)
                        printf("%c" , DATA[i]);
                }

                printf("\n\n");
                break;
            }
        }
    }

    printf(T"|-----------------------------------------------------------------------------------------------------|\n\n"E);
    return SUCCESS;
}

