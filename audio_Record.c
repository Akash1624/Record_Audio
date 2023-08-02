
/*
Write a C/C++ program that records samples from your computer microphone at 58 kHz sampling rate for 4.5 seconds.
Save a graph/plot the recorded sound.
*/

#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLING_FREQ 58000
#define CHANNELS 1
#define DURATION 4.5

int input_audio(const void *in_buff, void *out_buff, unsigned long frames_per_buff, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *data)
{
    float *input = (float *)in_buff;
    FILE *file = (FILE *)data;
    for (unsigned int i = 0; i < frames_per_buff; i++)
    {
        fprintf(file, "%f\n", *input++);
    }
    return paContinue;
}

int main()
{
    PaError error;
    FILE *file;

    // Initialize PortAudio
    error = Pa_Initialize();
    if (error != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(error));
        return 1;
    }
    PaStream *stream;
    error= Pa_OpenDefaultStream(&stream, CHANNELS, CHANNELS, paFloat32,
                               SAMPLING_FREQ, paFramesPerBufferUnspecified, input_audio, file);
    if (error != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }
    file = fopen("recorded_samples.txt", "w");
    if (file == NULL)
    {
        printf("Error: Unable to create the output file.\n");
        return 1;
    }
    error = Pa_StartStream(stream);
    if (error != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(error));
        Pa_Terminate();
        return 1;
    }

    Pa_Sleep(DURATION * 1000); 

    error = Pa_StopStream(stream);
    if (error != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(error));
        Pa_Terminate();
        return 1;
    }

    error = Pa_CloseStream(stream);
    if (error != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(error));
        Pa_Terminate();
        return 1;
    }

    fclose(file);

    Pa_Terminate();

    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe != NULL)
    {
        fprintf(gnuplotPipe, "set title 'Recorded Sound'\n");
        fprintf(gnuplotPipe, "set xlabel 'Sample'\n");
        fprintf(gnuplotPipe, "set ylabel 'Amplitude'\n");
        fprintf(gnuplotPipe, "set terminal png size 800,600\n");
        fprintf(gnuplotPipe, "set output 'recorded_sound.png'\n");
        fprintf(gnuplotPipe, "plot 'recorded_samples.txt' with lines\n");
        fprintf(gnuplotPipe, "exit\n");
        pclose(gnuplotPipe);
    }
    else
    {
        printf("Error: Gnuplot pipe could not be opened.\n");
    }

    return 0;
}
