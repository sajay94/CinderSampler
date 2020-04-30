#ifndef SAMPLE_CONTROLLER_H_
#define SAMPLE_CONTROLLER_H_

#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>

#include "../../../blocks/AudioUnit/include/AudioUnit.h"

using namespace ci;
using namespace std;
using namespace audio;
using namespace au;

namespace myapp {
    class SampleController {
        public:
            SampleController();
            void setup(const fs::path asset);
            void draw();
            PolyLine2 drawWave();
            void setReverb(float value);
            void update();


            au::GenericUnit reverb;
            au::SpeechSynth speechSynth;
            au::Mixer mixer;
            au::Output output;
            au::FilePlayer fileToPlay;
            au::Tap tap;
            TapSampleBuffer tapBuffer;
    };

}
#endif  // SAMPLE_CONTROLLER_H_