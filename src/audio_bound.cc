
#include "audio_bound.h"
#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>


#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"

#include "AudioUnit/AudioUnit.h"


namespace myapp {

    using namespace ci;
    using namespace ci::app;
    using namespace std;
    using namespace au;

    AudioBound::AudioBound() {}
    void AudioBound::setUp(size_t frame, size_t frames) {
        currentPosition = frame;
        numFrames = frames;
    }
    void AudioBound::setPosition(size_t frame) {
        currentPosition = frame;
    }
    void AudioBound::draw() {
        float readPos = (.875) * (float)getWindowWidth() * currentPosition / numFrames;
        gl::color( ColorA( 1, .6, 0, 0.7f ) );
        gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 200 ) );
    }

    int AudioBound::getXPosition() {
        float xPos = (.875) * (float)getWindowWidth() * currentPosition / numFrames;
        return xPos + 50;
    }
}

