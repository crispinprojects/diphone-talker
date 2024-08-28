/***************************************************************************
 *   Author Alan Crispin                 *
 *   crispinalan@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/



#ifndef WAVAMP_H
#define WAVAMP_H

//Wav file format

/*
 *   -------------------
 *   | Chunk | RIFF   |
 *    ------------------
 *   | Chunk | FMTHDR |
 *   -------------------
 *   | Chunk | FMT    |
 *   -------------------
 *   | Chunk | DATA   |
 *   -------------------
*/

// 0 - 4	“RIFF”	Marks the file as a riff file. Characters are each 1 byte long.
// 4 - 8	File size (integer)	Size of the overall file - 8 bytes (32-bit integer).
// 8 -12	“WAVE”	File Type Header. Always equals “WAVE”.
// 12-16	Sub_Chunk1 ID = “fmt "	Format chunk marker. Includes trailing null
// 16-20	Sub_Chunk1 Size -size of format data
// 20-22	AudioFormat (1 is PCM) - 2 byte integer
// 22-24	Number of Channels - 2 byte integer
// 24-28	Sample Rate - 32 byte integer. Sample Rate = Number of Samples per second
// 28-32	ByteRate	(Sample Rate * BitsPerSample * Channels) / 8
// 32-34	BlockAlign (BitsPerSample * Channels)
// 34-36	Bits per sample
// 36-40	Sub_Chunk2 ID = Data chunk header.  Marks the start of the data section.
// 40-44	SubChunk2 Size -size of the data section
// 44-End   Actual data


//wav specification
// https://sites.google.com/site/musicgapi/technical-documents/wav-file-format



#include <string>
#include <vector>       // std::vector
#include <stdint.h>     // int32_t, int16_t
#include <iostream>
#include <fstream>      //for *.c_str() type
#include <cstring>     //needed for memset used in this implementation


class WavAmp
{
public:
    WavAmp();
    virtual ~WavAmp();

    //structures

    //header RIFF chunk
    struct RIFF
    {
        char riffID[4];   //4 bytes
        int32_t riffSIZE;
        char riffFORMAT[4];
    };


    //format chunk header
    struct FMTHDR
    {
        char fmtID[4];      //4
        int32_t fmtSIZE;    //4
    };
    // format chunk
    struct FMT
    {
        int16_t wFormatTag;
        int16_t nChannels;
        int32_t nSamplesPerSec;
        int32_t nAvgBytesPerSec;
        int16_t nBlockAlign;
        int16_t wBitsPerSample;
    };

    //data chunk
    struct DATA
    {
        char dataID[4];     // 4 bytes
        int32_t dataSIZE;   // 4 bytes
    };

    static const int RIFF_SIZE      = 12;
    static const int FMTHDR_SIZE    = 8;
    static const int FMT_SIZE       = 16;
    static const int DATA_SIZE      = 8; //2 x4

    void wavAmp(std::string filename_in, std::string filename_out, double gain);

private:


    RIFF        riff;
    FMTHDR      fmthdr;
    FMT         fmt;
    DATA        data; //data structure

};


#endif // WAVAMP_H

