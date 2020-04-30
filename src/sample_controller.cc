
#include "sample_controller.h"

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

    SampleController::SampleController() {

    }

    void SampleController::setup(const fs::path asset) {
        fileToPlay.setFile(app::loadAsset("out2.wav"));
        reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
        fileToPlay.connectTo(reverb).connectTo(tap).connectTo(mixer).connectTo(output);
        fileToPlay.loop(1, 0);
        output.start();
    }

    void SampleController::draw() {
        fileToPlay.play(0);
        const cinder::PolyLine2 poly = drawWave();
        gl::draw(poly);
    }


    PolyLine2 SampleController::drawWave() {
        int width = 700;
        int height = 700;
        // Taken from Cinder-AudioUnit samples :
        // https://github.com/admsyn/Cinder-AudioUnit/blob/master/samples/auComplexRouting/src/auComplexRoutingApp.cpp
        PolyLine2f waveform;

        if(!tapBuffer.empty()) {
            waveform.getPoints().reserve(tapBuffer.size());

            const float xStep = width / (float)tapBuffer.size();

            for(int i = 0; i < tapBuffer.size(); i++) {
                float x = i * xStep;
                float y = tapBuffer[i] * (height / 2.f) + (height / 2.f);
                ci::vec2 newPoint(x, y);
                std::cout << tapBuffer[i] << std::endl;
                waveform.push_back(newPoint);
            }
        }

        return waveform;
    }

    void SampleController::update() {
        tap.getSamples(tapBuffer);
    }
}