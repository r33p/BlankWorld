#include <iostream>
#include <RtAudio.h>

// Two-channel sawtooth wave generator.
int sawtooth(char *buffer, int bufferSize, void *data)
{
  int i, j;
  double *my_buffer = (double *) buffer;
  double *my_data = (double *) data;

  // Write interleaved audio data.
  for (i=0; i<bufferSize; i++) {
    for (j=0; j<2; j++) {
      *my_buffer++ = my_data[j];

      my_data[j] += 0.005 * (j+1+(j*0.1));
      if (my_data[j] >= 1.0) my_data[j] -= 2.0;
    }
  }

  return 0;
}

int main()
{
  int channels = 2;
  int sampleRate = 44100;
  int bufferSize = 256;  // 256 sample frames
  int nBuffers = 4;      // number of internal buffers used by device
  int device = 0;        // 0 indicates the default or first available device
  double data[2];
  char input;
  RtAudio *audio = 0;

  // Open a stream during RtAudio instantiation
  try {
    audio = new RtAudio(device, channels, 0, 0, RTAUDIO_FLOAT64,
                        sampleRate, &bufferSize, nBuffers);
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  try {
    // Set the stream callback function
    audio->setStreamCallback(&sawtooth, (void *)data);

    // Start the stream
    audio->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get(input);

  try {
    // Stop and close the stream
    audio->stopStream();
    audio->closeStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  delete audio;

  return 0;
}
