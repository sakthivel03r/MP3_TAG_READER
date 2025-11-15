#ifndef MP3_H
#define MP3_H

#include <stdio.h>

/*------------------------------------------------------------------------------
 * Structure: SongInfo
 * Purpose  : Holds all information related to input MP3 file, temporary file,
 *            and tag/frame sizes used during parsing and editing.
 *----------------------------------------------------------------------------*/
typedef struct
{
    char * songname;          /* Name of the original MP3 file                 */
    FILE * fptr_songname;     /* File pointer to the original MP3 file         */
    char * Tempsongname;      /* Temporary file name used during editing       */
    FILE * fptr_Tempname;     /* File pointer to the temporary file            */
    char Tag_size;            /* Total number of standard ID3 frames supported */
    long int FrameSize;       /* Size of the current frame being processed     */

}SongInfo;

/*------------------------------------------------------------------------------
 * Union: Data
 * Purpose : Used for reading/writing 32-bit sizes by accessing them either as
 *           an integer or as a 4-byte character array (useful for syncsafe).
 *----------------------------------------------------------------------------*/
typedef union
{
    int  size;                /* Integer format of the value                   */
    char arr[4];              /* Byte-wise access to the same integer          */

}Data;

extern Data data;

/*------------------------------------------------------------------------------
 * Enumeration: outcome
 * Purpose     : Standard return status and operation modes for the program.
 *----------------------------------------------------------------------------*/
typedef enum
{
    FAILURE ,                 /* Generic failure response                      */
    SUCCESS ,                 /* Generic success response                      */
    VIEW    ,                 /* Operation mode: View MP3 tags                 */
    EDIT    ,                 /* Operation mode: Edit MP3 tags                 */
    TITTLE  ,                 /* Frame type: Title (TIT2)                      */
    ARTIST  ,                 /* Frame type: Artist (TPE1)                     */
    ALBUM   ,                 /* Frame type: Album  (TALB)                     */
    YEAR    ,                 /* Frame type: Year   (TYER)                     */
    CONTENT ,                 /* Frame type: Genre/Content                     */
    COMMENT                   /* Frame type: Comment (COMM)                    */

}outcome;

/*------------------------------------------------------------------------------
 * Function Prototypes
 *----------------------------------------------------------------------------*/
outcome Validate_Argument(char * argv[]);                         /* Validate CLI arguments       */
outcome Check_operation_Type(char Type , char * argv[]);          /* Determine operation mode     */
outcome Open_File(char Type , char FileName[] , SongInfo *);      /* Open input/output files      */
outcome View_Details_Mp3(SongInfo * viewinfo);                    /* View MP3 tag information     */
outcome Edit_Details_Mp3(SongInfo * editinfo , char Data[] ,
                         char * Key);                             /* Edit specific MP3 tag frame  */

void Help(char * FileName);                                       /* Display help message         */

/*------------------------------------------------------------------------------
 * Byte Conversion Utilities
 *----------------------------------------------------------------------------*/

/* Conversion: LSB <-> MSB for 4-byte values */
void Converstion(char arr[]);

/* Decode syncsafe integer (7-bit bytes used in ID3v2 headers) */
unsigned int read_syncsafe(unsigned char Byte[4]);

/* Write integer into 4-byte syncsafe format */
void write_syncsafe(unsigned int size, unsigned char out[4]);

/*------------------------------------------------------------------------------
 * Colour Codes for Terminal Output
 *----------------------------------------------------------------------------*/
#define R "\033[1;91m"              /* Red     */
#define G "\033[1;92m"              /* Green   */
#define B "\033[1;94m"              /* Blue    */
#define T "\033[38;2;0;206;209m"    /* Teal    */
#define O "\033[38;2;255;140;0m"    /* Orange  */
#define Y "\033[38;2;255;215;0m"    /* Yellow  */
#define E "\033[0m"                 /* Reset   */

#endif

