// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>

#include "../../../blocks/AudioUnit/include/AudioUnit.h"


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
    void fileDrop( FileDropEvent event ) override;
    void exportFile();
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void draw() override;

    GainNodeRef	mGain;
    BufferPlayerNodeRef mBufferPlayerNode;
    SourceFileRef mSourceFile;

    au::GenericUnit reverb;
    au::SpeechSynth speechSynth;
    au::Mixer mixer;
    au::Output output;
    au::Tap tap;
    WaveformPlot mWaveformPlot;

 private:
    cinder::audio::VoiceRef guide_audio_;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
