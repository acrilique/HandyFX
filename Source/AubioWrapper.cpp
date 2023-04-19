#include "AubioWrapper.h"

AubioWrapper::AubioWrapper()
{
    input = nullptr;
    tempo = nullptr;
    detected_bpm = 0.0f;
}

AubioWrapper::AubioWrapper(int sampleRate, int bufferSize)
{
	input = new_fvec(bufferSize);
	tempo = new_aubio_tempo("specflux", bufferSize, bufferSize / 2, sampleRate);
	detected_bpm = 0.0f;
}

AubioWrapper::~AubioWrapper()
{
	del_aubio_tempo(tempo);
	del_fvec(input);
}

bool AubioWrapper::initialiseWrapper(int sampleRate, int bufferSize)
{
    if (tempo != nullptr)
    {
        del_aubio_tempo(tempo);
    }

	input = new_fvec(bufferSize);
	tempo = new_aubio_tempo("specflux", bufferSize, bufferSize / 2, sampleRate);
    if (tempo == nullptr)
    {
        // Handle the error, for example, by logging an error message
        DBG ("Error: could not create aubio tempo object");
        return false;
    }
	detected_bpm = 0.0f;
	return true;
}

void AubioWrapper::detectBeats(const juce::AudioBuffer<float>& buffer)
{
    if (tempo == nullptr)
    {
        // Handle the error, for example, by logging an error message
        DBG ("Error: aubio tempo object wasnt created when trying to detect beats.");
        initialiseWrapper(48000,buffer.getNumSamples());
        return;
    }

    // Check if the buffer is empty

    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
    {
		return;
	}

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    fvec_t* inBuffer = new_fvec(numSamples);
    fvec_t* outTempo = new_fvec(2);

    for (int i = 0; i < numSamples; ++i)
    {
        float sum = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            sum += buffer.getReadPointer(ch)[i];
        }
        inBuffer->data[i] = sum / static_cast<float>(numChannels);
    }

    aubio_tempo_do(tempo, inBuffer, outTempo);

    // Check if a beat was detected
    if (outTempo->data[0] != 0)
    {
        detected_bpm = aubio_tempo_get_bpm(tempo);
    }

    del_fvec(inBuffer);
    del_fvec(outTempo);
}


float AubioWrapper::getDelayTime(float division)
{
    // ensure the bpm is between 80 and 160, if bigger divide by 2, if smaller multiply by 2
    if (detected_bpm > 160.0f)
		detected_bpm /= 2.0f;
	else if (detected_bpm < 80.0f)
		detected_bpm *= 2.0f;
	// division should be 1/16, 1/8, 1/4, 1/2, 3/4, 1, 2 or 4
	// if the detected bpm is 0, return 1.0f
        
    if (detected_bpm == 0.0f)
		return 1.0f;

    return (60.0f / detected_bpm) * division;
}

float AubioWrapper::getBPM()
{
	return detected_bpm;
}