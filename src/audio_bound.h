#ifndef AUDIO_BOUND_H_
#define AUDIO_BOUND_H_

#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>

#include "../../../blocks/AudioUnit/include/AudioUnit.h"

using namespace ci;
using namespace std;
using namespace audio;


namespace myapp {

    class AudioBound {
    public:
        AudioBound();
        void setUp(size_t frame, size_t frames);
        void setPosition(size_t frame);
        int getXPosition();
        void draw();

        size_t currentPosition;
        size_t numFrames;
    };
}
#endif  // AUDIO_BOUND_H_