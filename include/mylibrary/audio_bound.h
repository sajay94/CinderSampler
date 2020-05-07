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
        // setups AudioBound with currentPosition frame and numFrames frames
        void setUp(size_t frame, size_t frames);
        void setPosition(size_t frame);
        int getXPosition();
        int getCurrentFrame();
        int getNumFrames();
        // draws rectagular bound at current x position on respective AudioBufferPlayer
        void draw();

        size_t currentPosition = 0;
        size_t numFrames;
        bool wasMoved;
        const double kWidgetToWindowRatio = .875;
    };
}
#endif  // AUDIO_BOUND_H_