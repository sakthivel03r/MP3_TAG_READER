#include <stdio.h>
#include <string.h>
#include "mp3.h"

void Copy_Remaining_Data(SongInfo * editinfo);

extern char * symbol[];
extern char * tag[];

/*------------------------------------------------------------------------------
 * Function : Edit_Details_Mp3
 * Purpose  : Locate a specific ID3v2 frame, update its content, rewrite the tag,
 *            and update the overall tag size in syncsafe format.
 *----------------------------------------------------------------------------*/
outcome Edit_Details_Mp3(SongInfo * editinfo , char Text[] , char * Key)
{
    printf(T"\n|----------------------------------------!"B" EDIT Mode of MP3 "E""T"! ----------------------------------------|\n\n"E);
    printf("MP3 FILE "B"< %s >"E" TAG SIZE FROM HEADER (syncsafe) :"O" %ld"E" Bytes\n\n",
            editinfo->songname, editinfo->FrameSize);

    char TAG[4];

    /*--------------------------------------------------------------
     * Identify which tag the user selected based on symbol[] array.
     *--------------------------------------------------------------*/
    for(char i = 0 ; i < editinfo->Tag_size ; i++)
    {
        if(!strcmp(Key, symbol[i]))
        {
            strcpy(TAG , tag[i]);      /* Map -t → TIT2 etc. */
            break;
        }
    }

    printf("User Choosen Tag "B"< %s >"E" with Data "B"< %s >\n\n"E , TAG , Text);

    char Tag[4];
    unsigned char length = strlen(Text), Size;

    /*--------------------------------------------------------------
     * 1. Copy 10-byte MP3 Header (ID3) to temp file
     *--------------------------------------------------------------*/
    fseek(editinfo->fptr_songname , 0 , SEEK_SET);

    char Header[10];
    fread(Header , 10 , 1 , editinfo->fptr_songname);
    fwrite(Header , 10 , 1 , editinfo->fptr_Tempname);

    /*--------------------------------------------------------------
     * 2. Loop through frames until reaching end of tag area
     *--------------------------------------------------------------*/
    while(editinfo->FrameSize + 10 > ftell(editinfo->fptr_songname))
    {
        /*----------------------------------------------------------
         * Read frame tag (TPE1/TIT2/TALB/etc.)
         *----------------------------------------------------------*/
        fread(Tag , 4 , 1 , editinfo->fptr_songname);
        fwrite(Tag , 4 , 1 , editinfo->fptr_Tempname);

        /*----------------------------------------------------------
         * If empty frame or APIC (image), stop processing
         *----------------------------------------------------------*/
        if(Tag[0] == 0 || !strncmp(Tag , "APIC" , 4))
            break;

        /*----------------------------------------------------------
         * Read + convert frame size
         *----------------------------------------------------------*/
        fread(&data.size, 4, 1, editinfo->fptr_songname);
        Converstion(data.arr);
        Size = data.size;

        /*----------------------------------------------------------
         * If this is the tag to modify → write new size
         * Else → write original size
         *----------------------------------------------------------*/
        if(!strncmp(Tag , TAG , 4))
        {
            data.size = length + 1;            /* +1 for NULL byte */
            Converstion(data.arr);
            fwrite(data.arr , 4 , 1 , editinfo->fptr_Tempname);
        }
        else
        {
            Converstion(data.arr);
            fwrite(data.arr , 4 , 1 , editinfo->fptr_Tempname);
        }

        /*----------------------------------------------------------
         * Copy frame flags (2 bytes)
         *----------------------------------------------------------*/
        char Flag[2];
        fread(Flag , 2 , 1 , editinfo->fptr_songname);
        fwrite(Flag , 2 , 1 , editinfo->fptr_Tempname);

        /*----------------------------------------------------------
         * If match → write new content and skip old content
         *----------------------------------------------------------*/
        if(!strncmp(Tag , TAG , 4))
        {
            fputc(0 , editinfo->fptr_Tempname);         /* NULL encoding byte */
            fwrite(Text , length , 1 , editinfo->fptr_Tempname);

            fseek(editinfo->fptr_songname , Size , SEEK_CUR);   /* Skip old data */
            break;                                              /* Done editing */
        }
        else
        {
            /*------------------------------------------------------
             * Copy unmodified frame data
             *------------------------------------------------------*/
            char buffer[Size];
            fread(buffer , Size , 1 , editinfo->fptr_songname);
            fwrite(buffer , Size , 1 , editinfo->fptr_Tempname);
        }
    }

    /*--------------------------------------------------------------
     * If APIC encountered before finding tag → user tag not present
     *--------------------------------------------------------------*/
    if(!strncmp(Tag , "APIC" , 4))
    {
        printf("Given Tag Reader "B"< %s >"E" Not Found In This "B"< %s >"E" File\n\n",
                TAG, editinfo->songname);
        return FAILURE;
    }

    /*--------------------------------------------------------------
     * Copy rest of file content after tag frames
     *--------------------------------------------------------------*/
    Copy_Remaining_Data(editinfo);

    /*--------------------------------------------------------------
     * Update total tag size in temp file
     *--------------------------------------------------------------*/
    fseek(editinfo->fptr_Tempname , 6 , SEEK_SET);

    editinfo->FrameSize = editinfo->FrameSize - Size + length + 1;

    printf("After Changing Meta Data Header "B"Tag Frame Size"E" is "O"%ld Bytes\n\n"E,editinfo->FrameSize);

    char Framesize[4] = {0,0,0,0};

    fread(Framesize , 4 , 1 , editinfo->fptr_Tempname);
    write_syncsafe(editinfo->FrameSize , Framesize);
    fwrite(Framesize , 4 , 1 , editinfo->fptr_Tempname);

    printf("Tag "B"< %s >"E" Update with Given Data "B"< %s >"E" Successfull\n\n",TAG, Text);
    printf(T"|-----------------------------------------------------------------------------------------------------|\n\n"E);

    return SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function : Copy_Remaining_Data
 * Purpose  : Copies entire MP3 data after tag frames from original to temp file.
 *----------------------------------------------------------------------------*/
void Copy_Remaining_Data(SongInfo * editinfo)
{
    int ch;
    while((fread(&ch , 1 , 1 , editinfo->fptr_songname)))
        fwrite(&ch , 1 , 1 , editinfo->fptr_Tempname);
}

