#ifndef AUDIO_BOUND_H_
#define AUDIO_BOUND_H_

#include <cinder/audio/audio.h>
#include "../../../../samples/_audio/BufferPlayer/include/Resources.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"


using namespace ci;
using namespace std;
using namespace audio;


namespace myapp {

    class AudioBound {
        // Holds a position on an audio player
    public:
        AudioBound();
        void setUp(size_t frame, size_t frames);
        void reset(size_t frame, size_t frames);
        void setPosition(size_t frame);
        int getXPosition();
        int getNumFrames();
        void draw();

        size_t currentPosition;
        size_t numFrames;
        bool wasMoved;
        const double kWidgetToWindowRatio = .875;
    };
}
#endif  // AUDIO_BOUND_H_