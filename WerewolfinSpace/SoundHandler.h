#pragma once
#include "stdafx.h"
#include <MMSystem.h>
#include <dsound.h>
#include <stdio.h>
class SoundHandler
{
private:
    struct WaveHeaderType
    {
        char chunkId[4];
        unsigned long chunkSize;
        char format[4];
        char subChunkId[4];
        unsigned long subChunkSize;
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned long sampleRate;
        unsigned long bytesPerSecond;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
        char dataChunkId[4];
        unsigned long dataSize;
    };
public:
    SoundHandler();
    bool initialize(HWND hWnd, string waveFile);
    void shutdown();
    ~SoundHandler();
    
    bool playLoop();
    bool playSingle();
private:
    bool initializeDirectSound(HWND hWnd);
    void shutdownDirectSound();
    bool loadWaveFile(char*, IDirectSoundBuffer8**);
    void shutdownWaveFile(IDirectSoundBuffer8**);

    

    IDirectSound8* m_DirectSound; 
    IDirectSoundBuffer* m_primaryBuffer;
    IDirectSoundBuffer8* m_secondaryBuffer1;

    
};

//http://rastertek.com/dx10tut14.html