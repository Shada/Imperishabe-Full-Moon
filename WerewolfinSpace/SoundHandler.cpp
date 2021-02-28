#include "SoundHandler.h"


SoundHandler::SoundHandler()
{
    m_DirectSound = 0;
    m_primaryBuffer = 0;
    m_secondaryBuffer1 = 0;
}
bool SoundHandler::initialize(HWND hWnd, string waveFile)
{
    bool result;

    //init sound and primary buffer
    result = initializeDirectSound(hWnd);
    if(!result)
        return false;

    //load a wavefile onto secondary buffer
    waveFile = "Sounds/" + waveFile;
    result = loadWaveFile((char*)waveFile.c_str(), &m_secondaryBuffer1);
    if(!result)
        return false;

    return true;
}

bool SoundHandler::initializeDirectSound(HWND hWnd)
{
    HRESULT result;
    DSBUFFERDESC bufferDesc;
    WAVEFORMATEX waveFormat;

    //initialize direct sound interface pointer for the default sound device
    result = DirectSoundCreate8(NULL,&m_DirectSound,NULL);
    if(FAILED(result))
        return false;

    //set cooplevel to priority so the format of the primary sound buffer can be modified
    result = m_DirectSound->SetCooperativeLevel(hWnd,DSSCL_PRIORITY);
    if(FAILED(result))
        return false;

    //setup the primary buffer description
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = 0;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = NULL;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    //get control of the primary buffer on the default sound device
    result = m_DirectSound->CreateSoundBuffer(&bufferDesc,&m_primaryBuffer,NULL);
    if(FAILED(result))
        return false;

    //setup the format of the primary sound buffer
    // In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8)* waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    //set the primary buffer to be the wave format specified
    result = m_primaryBuffer->SetFormat(&waveFormat);
    if(FAILED(result))
        return false;

    return true;

}

bool SoundHandler::loadWaveFile(char* filename,IDirectSoundBuffer8** secondaryBuffer)
{
    int error;
    FILE* filePtr;
    unsigned int count;
    WaveHeaderType waveFileHeader;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    unsigned char* waveData;
    unsigned char* bufferPtr;
    unsigned long bufferSize;

    //open wave file in binary
    error = fopen_s(&filePtr,filename,"rb");
    if(error != 0)
        return false;

    //read wave header
    count = fread(&waveFileHeader,sizeof(waveFileHeader),1,filePtr);
    if(count != 1)
        return false;

    //check so that the chunk ID is in the RIFF format
    if(    (waveFileHeader.chunkId[0] != 'R')    || (waveFileHeader.chunkId[1] != 'I') ||
        (waveFileHeader.chunkId[2] != 'F')    || (waveFileHeader.chunkId[3] != 'F'))
        return false;

    //check so that the file is in the wave format
    if(    (waveFileHeader.format[0] != 'W')    || (waveFileHeader.format[1] != 'A') ||
        (waveFileHeader.format[2] != 'V')    || (waveFileHeader.format[3] != 'E'))
        return false;

    //check so that the sub chunk ID is in the fmt format
    if(    (waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
        (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
        return false;

    //check so that the audio format is WAVE_FORMAT_PCM
    if( waveFileHeader.audioFormat != WAVE_FORMAT_PCM ) 
        return false;

    //check so that the wave file is in stereo format
    /*if(waveFileHeader.numChannels != 2)
        return false;*/

    //check so that the file has a sample rate of 44.1KHz
    /*if(waveFileHeader.sampleRate != 44100)
        return false;*/

    //check so that the file is in 16 bit format
    if(waveFileHeader.bitsPerSample != 16)
        return false;

    // Check for the data chunk header.
    if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
       (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
       return false;

    // Set the wave format of secondary buffer that this wave file will be loaded onto.
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
 
    // Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    //create a temporary buffer with the specific buffer settings
    result = m_DirectSound->CreateSoundBuffer(&bufferDesc,&tempBuffer,NULL);
    if(FAILED(result))
        return false;

    //test buffer format against the direct sound 8 interface and create secondary buffer
    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void**)&*secondaryBuffer);
    if(FAILED(result))
        return false;

    //release tempbuffer
    tempBuffer->Release();
    tempBuffer = 0;

    // Move to the beginning of the wave data which starts at the end of the data chunk header.
    fseek(filePtr,sizeof(WaveHeaderType),SEEK_SET);

    //create a temporary buffer to hold the wave file data
    waveData = new unsigned char[waveFileHeader.dataSize];
    if(!waveData)
        return false;

    //read in the wave file data into the newly created buffer
    count = fread(waveData,1,waveFileHeader.dataSize,filePtr);
    if(count != waveFileHeader.dataSize)
        return false;

    //close the file once done reading
    error = fclose(filePtr);
    if(error != 0 )
        return false;

    //lock the secondary buffer to write wave data into it
    result = (*secondaryBuffer)->Lock(0,waveFileHeader.dataSize,(void**)&bufferPtr,(DWORD*)&bufferSize,NULL,0,0);
    if(FAILED(result))
        return false;

    //copy the wave data into the buffer
    memcpy(bufferPtr,waveData,waveFileHeader.dataSize);

    //unlcok the secondary buffer after the data has been written to it
    result = (*secondaryBuffer)->Unlock((void*)bufferPtr,bufferSize,NULL,0);
    if(FAILED(result))
        return false;

    //release the wave data since it was copied into  the secondary buffer
    delete[] waveData;
    waveData = 0;

    return true;
}

bool SoundHandler::playLoop()
{
    HRESULT result;

    //set position at the beginning of the soundbuffer
    result = m_secondaryBuffer1->SetCurrentPosition(0);
    if(FAILED(result))
        return false;

    //set volume of the buffer to 100%
    result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
    if(FAILED(result))
        return false;

    //Play the contents of the secondary sound buffer
    result = m_secondaryBuffer1->Play(0,0,DSBPLAY_LOOPING);
    if(FAILED(result))
        return false;

    return true;
}

bool SoundHandler::playSingle()
{
    HRESULT result;

    //set position at the beginning of the soundbuffer
    result = m_secondaryBuffer1->SetCurrentPosition(0);
    if(FAILED(result))
        return false;

    //set volume of the buffer to 100%
    result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
    if(FAILED(result))
        return false;

    //Play the contents of the secondary sound buffer
    result = m_secondaryBuffer1->Play(0,0,0);
    if(FAILED(result))
        return false;

    return true;
}

void SoundHandler::shutdown()
{
    //release secondary buffer
    shutdownWaveFile(&m_secondaryBuffer1);

    //shutdown direct sound API
    shutdownDirectSound();
}
void SoundHandler::shutdownDirectSound()
{
    //release primary soundbuffer pointer
    if(m_primaryBuffer)
    {
        m_primaryBuffer->Release();
        m_primaryBuffer = 0;
    }
    //release the direct sound interface pointer
    if(m_DirectSound)
    {
        m_DirectSound->Release();
        m_DirectSound = 0;
    }
}

void SoundHandler::shutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
    //release the secondary buffer
    if(*secondaryBuffer)
    {
        (*secondaryBuffer)->Release();
        *secondaryBuffer = 0;
    }
}
SoundHandler::~SoundHandler()
{
    /*if(m_DirectSound)
        SAFE_RELEASE(m_DirectSound);
    if(m_primaryBuffer)
        SAFE_RELEASE(m_primaryBuffer);
    if(m_secondaryBuffer1)
        SAFE_RELEASE(m_secondaryBuffer1);*/
    shutdown();
}
