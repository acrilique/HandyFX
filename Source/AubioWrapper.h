
#ifndef AUBIOWRAPPER_H
#define AUBIOWRAPPER_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <aubio/aubio.h>
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
class AubioWrapper
{
public:

	AubioWrapper();
	AubioWrapper(int sampleRate, int bufferSize);
	~AubioWrapper();
	bool initialiseWrapper(int sampleRate, int bufferSize);
	void detectBeats(const juce::AudioBuffer<float>& buffer);
	float getDelayTime(float division);

private:

	fvec_t* input;
	aubio_tempo_t* tempo;
	float detected_bpm;

};

#endif
