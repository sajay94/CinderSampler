// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/audio/audio.h>
#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include <../../../samples/_audio/common/AudioDrawUtils.h>
#include "cinder/audio/cocoa/ContextAudioUnit.h"
#include "../../../blocks/Cinder-UI/src/UI.h"
#include "mylibrary/audio_bound.h"
#include "mylibrary/sample_controller.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace audio;
using namespace reza::ui;
using namespace audio::cocoa;

namespace myapp {

class AudioAligner : public cinder::app::App {
 public:
    AudioAligner();
    void setup() override;
    void fileDrop( FileDropEvent event ) override;
    void setUpSample();
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void draw() override;

    // Draws plot from player at size --> Used for both orginal and sampled audio
    void drawAudioPlayer(const Rectf& size, WaveformPlot plot, BufferPlayerNodeRef player);

    // Used to quickly load mUi default from json
    fs::path getSaveLoadPath();

 private:

    // For positioning AudioPlayer Widget
    const Rectf kAudioPosition = Rectf(50, 100, 750, 200);
    const Rectf kSamplePosition = Rectf(50, 250, 750, 350);
    const float kWindowToWidgetRatio = float(8/7);

    SuperCanvasRef mUi;
    float volume = 0.5;
    float pan = 0.0;

    // For the base sample that the user drags in
    BufferPlayerNodeRef mBufferPlayerNode;
    SourceFileRef mSourceFile;
    WaveformPlot mWaveformPlot;

    // Objects which allow the user to set the bounds of the sample
    myapp::AudioBound start = AudioBound();
    myapp::AudioBound end = AudioBound();

    // True if start or end were moved. Should be moved to audio_bound.h as bool moved
    bool moveStart = false;
    bool moveEnd = false;

    // Allows for multiple samples which can then be stringed and played together
    vector<SampleController> samples;
    int currentSample = -1;

    // True after setUpSample is called
    bool isSampleSelected = false;

};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
