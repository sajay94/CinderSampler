// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>

#include "../../../blocks/AudioUnit/include/AudioUnit.h"
#include "../src/audio_bound.h"
#include "../src/sample_controller.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace audio;

namespace myapp {

class AudioAligner : public cinder::app::App {
 public:
    AudioAligner();
    void setup() override;
    void resize() override;
    void update() override;
    void fileDrop( FileDropEvent event ) override;
    void setUpSample();
    void setSample();
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void draw() override;
    void drawAudioPlayer();
    PolyLine2 drawSample();

    BufferPlayerNodeRef mBufferPlayerNode;
    SourceFileRef mSourceFile;
    WaveformPlot mWaveformPlot;
    myapp::AudioBound start = AudioBound();
    myapp::AudioBound end = AudioBound();
    bool moveStart = false;
    bool moveEnd = false;
    SampleController sample = SampleController();
    bool isSampleSelected = false;
    bool isReverbOn = true;
    bool isDistortionOn = true;
    GainNodeRef	mGain;

    au::GenericUnit reverb;
    au::GenericUnit distortion;
    au::Mixer mixer;
    au::Output output;
    au::FilePlayer fileToPlay;
    au::Tap tap;
    au::TapSampleBuffer tapBuffer;


};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
