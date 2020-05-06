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
    void update() override;
    void fileDrop( FileDropEvent event ) override;
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void draw() override;

    void setUpSample(BufferPlayerNodeRef player, SourceFileRef source, size_t bound_index, string filename);
    void setUpBounds(BufferPlayerNodeRef player);
    void setUpSampleBufferPlayer(Context* sampleCtx);
    void setUpSequencer();
    void setControls();
    void exportSequence();

    // Draws plot from player at size --> Used for both orginal and sampled audio
    void drawAudioPlayer(const Rectf& size, WaveformPlot plot, BufferPlayerNodeRef player);
    void drawSequencer();

    // Used to quickly load mUi default from json
    fs::path getSaveLoadPath();

 private:

    enum Stage {
        Clip,
        Sample,
        Sequence
    };

    Stage currentStage = Clip;
    // out files
    const string outPath = "/Users/family/Cinder/Projects/final-project-sajay94-sampler/assets/";

    // For positioning AudioPlayer Widget
    const Rectf kAudioPosition = Rectf(50, 100, 750, 200);
    const Rectf kSamplePosition = Rectf(50, 250, 750, 350);
    const Rectf kSequencerPosition = Rectf(50, 400, 750, 450);
    const float kWindowToWidgetRatio = float(8/7);

    SuperCanvasRef mUi;
    SuperCanvasRef mUiControls;
    float volume = 0.5;
    float pan = 0.5;
    float rev = 0.5;
    int low = 1600;
    int high = 0;
    int numSamples = 0;
    int kSequenceLimit = 16;

    // For the base sample that the user drags in
    BufferPlayerNodeRef mBufferPlayerNode;
    SourceFileRef mSourceFile;
    WaveformPlot mWaveformPlot;

    BufferPlayerNodeRef mSampleBufferPlayerNode;
    BufferRecorderNodeRef mExportRecorderNode;
    SourceFileRef mSampleSourceFile;
    WaveformPlot mSampleWaveformPlot;

    // Objects which allow the user to set the bounds of the sample
    myapp::AudioBound start = AudioBound();
    myapp::AudioBound end = AudioBound();

    // Objects which allow the user to fine tune the bounds of the sample
    myapp::AudioBound sampleStart = AudioBound();
    myapp::AudioBound sampleEnd = AudioBound();

    vector<AudioBound> bounds;

    // Allows for multiple samples which can then be stringed and played together
    vector<SampleController> samples;
    int currentSample = -1;

};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
