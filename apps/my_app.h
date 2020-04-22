// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace myapp {

class AudioAligner : public cinder::app::App {
 public:
    AudioAligner();
    void setup() override;
    void resize() override;
    void fileDrop( FileDropEvent event ) override;
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void draw() override;

    audio::GainNodeRef				mGain;
    audio::BufferPlayerNodeRef		mBufferPlayerNode;

    WaveformPlot				mWaveformPlot;

 private:
    cinder::audio::VoiceRef guide_audio_;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
