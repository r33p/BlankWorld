// source :
// www.stevematsumoto.net/svn/platitude/music/RtAudio/tests/record.cpp
// build :
// g++ -Wall -o "%e" "%f" -lrtaudio -std=c++11
//
// TODO/NOTES :
// optimiser le code avec memcpy car pour l'instant la copies'effectue
// dans une doucle boucle 'for'... Pas terrible...

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

typedef signed short  MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE  32767.0


signed short* buuff;

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData )
{
	unsigned int i, j;
	signed short *buffer = (signed short *) inputBuffer;
	//double *data = (double *) userData;
	
//	signed short *maData = (signed short *) userData;

	
	if ( status )
	std::cout << "Stream overflow detected!" << std::endl;
	// Do something with the data in the "inputBuffer" buffer.	
	
	for ( i=0; i<nBufferFrames; i++ ) {
		for ( j=0; j<2; j++ ) {
			//std::cout << (float)*buffer/100<< " ";
			buuff = buffer;
			std::cout << (float)*buuff/100<< " ";
			buuff++;
			buffer++;
		}
	}
	
	std::cout << "\n\n\n\n";
	return 0;
}

int main()
{
	RtAudio adc;
	if ( adc.getDeviceCount() < 1 ) {
		std::cout << "\nNo audio devices found!\n";
		exit( 0 );
	}
	
	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();
	
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 512; // 512 sample frames
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	
	try {
		adc.openStream( NULL, &parameters, RTAUDIO_SINT16,
		sampleRate, &bufferFrames, &record );
		adc.startStream();
	}
	catch ( RtAudioError& e ) {
		e.printMessage();
		exit( 0 );
	}
	char input;
	std::cout << "\nRecording ... press <enter> to quit.\n";
	std::cin.get( input );
	try {
		// Stop the stream
		adc.stopStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
	}
	if ( adc.isStreamOpen() ) adc.closeStream();
	return 0;
}
