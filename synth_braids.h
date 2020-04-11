#ifndef SYNTH_BRAIDS_H_
#define SYNTH_BRAIDS_H_

#include "AudioStream.h"
#include "utility/dspinst.h"
#include "macro_oscillator.h"


using namespace braids;

class AudioSynthBraids: public AudioStream
{
public:
        AudioSynthBraids(): AudioStream(1, inputQueueArray), kSampleRate(AUDIO_SAMPLE_RATE_EXACT), kAudioBlockSize(AUDIO_BLOCK_SAMPLES), magnitude(65536.0) { }
        ~AudioSynthBraids() { }

        void set_braids_shape(int16_t shape) {
      		shapebraids = shape;
          // Trims the shape to the valid values
          shapebraids = shapebraids >= MACRO_OSC_SHAPE_QUESTION_MARK ? MACRO_OSC_SHAPE_QUESTION_MARK : shapebraids<0 ? 0 : shapebraids;

          // Sets the shape
          MacroOscillatorShape osc_shape = static_cast<MacroOscillatorShape>(shapebraids);//
          osc.set_shape(osc_shape);
      	}

        void set_braids_color(int16_t colorbraids) {
      		color = colorbraids;
          osc.set_parameters(timbre,color);
      	}

        void set_braids_timbre(int16_t timbrebraids) {
      		timbre = timbrebraids;
          osc.set_parameters(timbre,color);
      	}

        void set_braids_pitch(int16_t pitchbraids) {
          pitch = pitchbraids;
          if(pre_pitch!=pitch){
    					osc.set_pitch(pitch);
    					pre_pitch = pitch;
    			}
      		osc.Strike();
      	}

        inline void init_braids(){
            // Global used to trigger the next buffer to render
            wait = 0;

            // Initializes the objects
            osc.Init();
            osc.set_shape(MACRO_OSC_SHAPE_CSAW);
            osc.set_parameters(0, 0);

            pitch = 32 << 7;
        }

        virtual void update(void);

private:
        audio_block_t *inputQueueArray[1];

        MacroOscillator osc;

        const uint32_t kSampleRate;
        const uint16_t kAudioBlockSize;
        //
        // Globals that define the parameters of the oscillator
        volatile int16_t pitch,pre_pitch;
        volatile int16_t timbre;
        volatile int16_t color;
        volatile int16_t shapebraids;

        int32_t magnitude;

        volatile uint8_t wait;

};

#endif
