#ifndef AUDIOGET_H
#define AUDIOGET_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>
#include <au_encode.h>
#include "comdef.h"
#include <QDebug>

/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (0)

/* Select sample format. */
//#if 1
//#define PA_SAMPLE_TYPE  paFloat32
//typedef float SAMPLE;
//#define SAMPLE_SILENCE  (0.0f)
//#define PRINTF_S_FORMAT "%.8f"
//#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
//#elif 0
//#define PA_SAMPLE_TYPE  paInt8
//typedef char SAMPLE;
//#define SAMPLE_SILENCE  (0)
//#define PRINTF_S_FORMAT "%d"
//#else
//#define PA_SAMPLE_TYPE  paUInt8
//typedef unsigned char SAMPLE;
//#define SAMPLE_SILENCE  (128)
//#define PRINTF_S_FORMAT "%d"
//#endif

typedef struct
{
    AVAudioFifo      *recordedSamples;
    AVAudioFifo      *receivedSamples;
}
paData;

static int audioCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paData *data = (paData*)userData;

    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    av_audio_fifo_write(data->recordedSamples,(void**)&inputBuffer,framesPerBuffer);

    if (av_audio_fifo_size(data->receivedSamples)>=framesPerBuffer)
    av_audio_fifo_read(data->receivedSamples,(void**)&outputBuffer,framesPerBuffer);

    return paContinue;
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/


class audioget : public QObject
{
    Q_OBJECT
public:
    explicit audioget(QObject *parent = 0);
    ~audioget();

    int init_audio(AVAudioFifo **auin_fifo, AVAudioFifo **auout_fifo);
    int record_audio();
    int close_audio();

    int                 numBytes;

private:

    PaStreamParameters  inputParameters,
                        outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paData              data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    SAMPLE              max, val;
    double              average;

signals:
    getaudio();

public slots:
};

#endif // AUDIOGET_H
