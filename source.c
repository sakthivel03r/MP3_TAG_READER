#include <stdio.h>
#include <string.h>
#include "mp3.h"

/*------------------------------------------------------------------------------
 * Function : Open_File
 * Purpose  : Opens MP3 file, validates ID3 header, reads total tag size,
 *            and prepares temporary file if in EDIT mode.
 *----------------------------------------------------------------------------*/
outcome Open_File(char Type , char FileName[] , SongInfo * viewinfo)
{
    viewinfo->songname = FileName;
    viewinfo->fptr_songname = fopen(viewinfo->songname , "r");   /* Open input MP3 */

    /*--------------------------------------------------------------
     * Validate: Was file successfully opened?
     *--------------------------------------------------------------*/
    if(viewinfo->fptr_songname == NULL)
    {
        printf(R "INFO : # ERROR #\n"E);
        printf("Given File "B"< %s >"E" is Not Open\n", FileName);
        return FAILURE;
    }

    char Header[4];

    /*--------------------------------------------------------------
     * Read first 3 bytes → Must be "ID3"
     *--------------------------------------------------------------*/
    fread(Header , 3 , 1 , viewinfo->fptr_songname);

    if(strncmp(Header , "ID3" , 3))
    {
        printf(R "INFO : # ERROR #\n"E);
        printf("Given File "B"< %s >"E" is Not a MP3 File\n", FileName);
        return FAILURE;
    }

    /*--------------------------------------------------------------
     * Seek to Tag size (bytes 6–9) and read syncsafe size
     *--------------------------------------------------------------*/
    fseek(viewinfo->fptr_songname , 6 , SEEK_SET);
    fread(Header , 1 , 4 , viewinfo->fptr_songname);

    viewinfo->FrameSize = read_syncsafe(Header);    /* Decode syncsafe size */
    rewind(viewinfo->fptr_songname);               /* Reset file position  */

    /*--------------------------------------------------------------
     * If EDIT mode: Prepare temp file for writing updated content
     *--------------------------------------------------------------*/
    if(Type == EDIT)
    {
        viewinfo->Tempsongname = "temp.mp3";
        viewinfo->fptr_Tempname = fopen(viewinfo->Tempsongname , "w");

        if(viewinfo->fptr_Tempname == NULL)
        {
            printf(R "INFO : # ERROR #\n"E);
            printf("Given File "B"< %s >"E" is Not Open\n", viewinfo->Tempsongname);
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function : read_syncsafe
 * Purpose  : Converts 4 syncsafe bytes (ID3v2 format) into a normal integer.
 *----------------------------------------------------------------------------*/
unsigned int read_syncsafe(unsigned char Byte[4])
{
    return ((Byte[0] & 0x7F) << 21) |
           ((Byte[1] & 0x7F) << 14) |
           ((Byte[2] & 0x7F) << 7)  |
           (Byte[3] & 0x7F);
}

/*------------------------------------------------------------------------------
 * Function : write_syncsafe
 * Purpose  : Converts integer to 4 syncsafe bytes (7-bit format).
 *----------------------------------------------------------------------------*/
void write_syncsafe(unsigned int size, unsigned char out[4])
{
    out[0] = (size >> 21) & 0x7F;
    out[1] = (size >> 14) & 0x7F;
    out[2] = (size >> 7)  & 0x7F;
    out[3] =  size        & 0x7F;
}

/*------------------------------------------------------------------------------
 * Function : Converstion
 * Purpose  : Reverses 4-byte array (LSB ↔ MSB) for endian conversion.
 *----------------------------------------------------------------------------*/
void Converstion(char arr[])
{
    for(char i = 0 ; i < 2 ; i++)
    {
        unsigned char temp = arr[i];
        arr[i] = arr[3 - i];
        arr[3 - i] = temp;
    }
}

/*------------------------------------------------------------------------------
 * Struct : Row
 * Purpose: Store one row of HELP menu (command + description).
 *----------------------------------------------------------------------------*/
typedef struct
{
    const char *command;
    const char *description;

} Row;

/*------------------------------------------------------------------------------
 * Function : Help
 * Purpose  : Displays a formatted help menu for usage instructions.
 *----------------------------------------------------------------------------*/
void Help(char * FileName)
{
    Row rows[] =
    {
        {" -v <Filename>",       "To View"},
        {" -e <Symbol> <Filename>", "To Edit"},
        {" -e -t < Filename >",  "To Edit Song Title"},
        {" -e -a < Filename >",  "To Edit Artist Name"},
        {" -e -A < Filename >",  "To Edit Album Name"},
        {" -e -y < Filename >",  "To Edit Year"},
        {" -e -c < Filename >",  "To Edit Content"},
        {" -e -m < Filename >",  "To Edit Comment"}
    };

    int n = sizeof(rows) / sizeof(rows[0]);

    printf(B"\n\n -------------------------! HELP MENU !-------------------------\n"E);
    printf(" _______________________________________________________________\n");

    for(char i = 0 ; i < n ; i++)
    {
        printf("|                                     |                         |\n");
        printf("| %-10s%-25s | %-23s |\n", FileName, rows[i].command, rows[i].description);
        printf("|_____________________________________|_________________________|\n");
    }

    printf("\n");
}

