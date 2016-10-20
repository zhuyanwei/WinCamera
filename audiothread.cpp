#include "audiothread.h"

#include<QDebug>

AudioThread::AudioThread(audioget *au)
{   
    audio = au;

    stopped = false;

}

AudioThread::~AudioThread()
{

}

void AudioThread::run()
{
    while(!stopped)
    {
        audio->record_audio();
    }
}

void AudioThread::stop()
{
    stopped = true;
}

