
#include "mylibrary/audio_bound.h"
#include <cinder/app/App.h>
#include <cinder/gl/gl.h>



namespace myapp {

    using namespace ci;
    using namespace ci::app;
    using namespace std;

    AudioBound::AudioBound() {}
    void AudioBound::setUp(size_t frame, size_t frames) {
        currentPosition = frame;
        numFrames = frames;
        wasMoved = false;
    }
    void AudioBound::setPosition(size_t frame) {
        currentPosition = frame;
    }
    void AudioBound::draw() {
        float readPos = kWidgetToWindowRatio * (float)getWindowWidth() * currentPosition / numFrames;
        gl::color( ColorA( 1, .6, 0, 0.7f ) );
        gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 100 ) );
    }

    int AudioBound::getXPosition() {
        int xPos = kWidgetToWindowRatio * (float)getWindowWidth() * currentPosition / numFrames;
        return xPos + 50;
    }

    int AudioBound::getNumFrames() {
        return numFrames;
    }

    int AudioBound::getCurrentFrame() {
        return currentPosition;
    }
}

