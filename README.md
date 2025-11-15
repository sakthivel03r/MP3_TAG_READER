-----------------------------------------------------------------------------------------------------------|
📌 Project Overview

MP3 TagReader is a command‑line tool written in C that allows users to view, edit, and manipulate ID3v2 metadata in MP3 audio files. The project focuses on understanding MP3 file structure, ID3 headers, tag frames, and safe editing of metadata without corrupting audio content.

This tool supports:

Viewing ID3v2 metadata (Artist, Title, Album, Year, Comment, etc.)

Editing specific tag frames

Reading MP3 headers and tag sizes

Handling sync‑safe integers

Modular and clean code architecture

------------------------------------------------------------------------------------------------------------|

How It Works

1. ID3v2 Header Handling

    The program reads the first 10 bytes of an MP3 file which contain:

    Signature: ID3

    Version & Flags

    Tag Size (sync‑safe integer)

2. Frame Parsing

    Each metadata element is stored in a frame, such as:

    TPE1 → Artist

    TIT2 → Title

    TALB → Album

    TYER → Year

    COMM → Comment

Your program:

Reads each frame ID

Determines frame size

Extracts or overwrites the text data

3. Viewing Metadata

    ViewDetailsMp3() lists all tags with proper formatting. It:

    Skips header bytes

    Iterates through frames

    Prints extracted text using frame mapping arrays (Info[], symbol[], tag[])

4. Editing Metadata

    EditDetailsMp3():

    Searches for the correct frame using the frame symbol table

    Updates the frame size when new text is inserted

    Shifts remaining data safely

    Writes back the edited MP3 with updated tag sizes

------------------------------------------------------------------------------------------------------------|

🔐 Sync-Safe Integer Handling (ID3v2)

ID3v2 uses sync-safe integers to avoid accidental MPEG sync patterns inside metadata. A sync-safe integer stores 7 bits per byte (MSB is always 0).

▶️ How Sync-Safe Works

A normal 32‑bit value:  0aaaaaaa 0bbbbbbb 0ccccccc 0ddddddd

Each byte can only store 7 bits, so total: 7 × 4 = 28 bits.

------------------------------------------------------------------------------------------------------------|

▶️ C Code to Convert Sync-Safe → Normal Integer

unsigned int decodesyncsafe(unsigned char b[4]) 
{
    return (b[0] << 21) |
    (b[1] << 14) |
    (b[2] << 7) |
    (b[3]);
}

C Code to Convert Normal → Sync-Safe

void encodesyncsafe(unsigned int value, unsigned char out[4]) 
{
    out[0] = (value >> 21) & 0x7F;
    out[1] = (value >> 14) & 0x7F;
    out[2] = (value >> 7) & 0x7F;
    out[3] = value & 0x7F;
}

This ensures no byte ever exceeds 0x7F, keeping the MP3 structure safe.

------------------------------------------------------------------------------------------------------------|

How Integers and Bytes Are Stored (LSB & MSB)

MP3 metadata relies heavily on byte ordering. Understanding how data is stored at the byte level is essential when parsing or editing ID3 tags.

------------------------------------------------------------------------------------------------------------|

LSB (Least Significant Byte) vs MSB (Most Significant Byte)

LSB-first (Little Endian): Lowest-value byte stored first in memory.

MSB-first (Big Endian): Highest-value byte stored first.

Most MP3 metadata (ID3v2) uses Big Endian for frame sizes, meaning:

Byte1 (MSB) Byte2 Byte3 Byte4 (LSB)

Example: A 4-byte size stored as: 00 00 02 10 → size = 0x210 = 528 bytes

------------------------------------------------------------------------------------------------------------|

Syncsafe Integers in ID3v2.3/2.4

A sync-safe integer is a 4‑byte value where each byte uses only 7 bits, preventing false MPEG sync patterns.

Example (original value → syncsafe → binary):

    Value: 420679680
    Syncsafe: 00 10 13 19
    Binary:
    00000000 00010000 00010011 00011001

------------------------------------------------------------------------------------------------------------|

How MP3 Metadata Is Stored Internally

ID3v2 metadata sits at the beginning of the file before the audio frames.

The structure looks like this:

+-------------------+
| ID3 Header (10B)  |
+-------------------+
| Frames (TIT2,     |
| TPE1, TALB, etc)  |
+-------------------+
| Padding (optional)|
+-------------------+
| MPEG Audio Data   |
+-------------------+

------------------------------------------------------------------------------------------------------------|

MP3 Version Used in This Project

Your project supports ID3v2.3, the most commonly used version in modern MP3 files.

ID3v2.3 contains:

Header (10 bytes)

Frames with 4-byte size fields (non-syncsafe)

Text encoding byte (00 = ISO-8859-1, 01 = UTF‑16)

No extended header by default

------------------------------------------------------------------------------------------------------------|
