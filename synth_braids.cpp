#include <Arduino.h>
#include "synth_braids.h"
#include "utility/dspinst.h"


void AudioSynthBraids::update(void)
{
	audio_block_t *block;
	uint32_t i, scale;
	int32_t val1, val2;
	const uint8_t sync_buffer[AUDIO_BLOCK_SAMPLES] = { 0 };
	uint8_t buffer_index = 0;

	if (magnitude) {
		block = allocate();
		if (block) {
			osc.Render(sync_buffer, buffer, AUDIO_BLOCK_SAMPLES);
			for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        val1 = buffer[buffer_index];
        if(buffer_index+1 > AUDIO_BLOCK_SAMPLES){
          val2 = 0;
        }else{
          val2 = buffer[buffer_index+1];
        }
				Serial.print(val1);
				Serial.print(",");
				if(i%8 == 0)
					Serial.print("\n");
				scale = (buffer_index >>16) & 0x7FFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
#if defined(__ARM_ARCH_7EM__)
				block->data[i] = multiply_32x32_rshift32(val1+val2, magnitude);
#elif defined(KINETISL)
				block->data[i] = (((val1+val2) >> 16) * magnitude) >> 16;
#endif
				buffer_index++;
			}
			Serial.print("\n");
			Serial.print("\n");
			transmit(block, 0);
			release(block);
      // wait = 0;
			return;
		}
	}
}
