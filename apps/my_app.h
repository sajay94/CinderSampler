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

class AudioSampler : public cinder::app::App {
 public:
    //Creates BufferPlayer from default file;
    AudioSampler();
    //Creates Cinder-UI elements which update respective private floats
    void setup() override;
    //Used to set the BufferPlayer to the dropped file;
    void fileDrop( FileDropEvent event ) override;
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void draw() override;

    //Creates clip of the sample in player and stores it in sample.wav
    void setUpSample(const BufferPlayerNodeRef& player, const SourceFileRef& source, size_t bound_index, const string& filename);
    // Creates bounds for SampleBufferPlayer
    void setUpBounds(const BufferPlayerNodeRef& player);

    //Creates the sample Buffer to be played based on sample.wav
    void setUpSampleBufferPlayer(Context* sampleCtx);
    // Creates 16 SampleControllers based on sample.wav
    void setUpSequencer();
    // Sets all UI variables to the values stored in SampleController. Called to show that SampleControllers controlls
    void setControls();
    // Used to update the current SampleController when some Cinder-UI element is activated
    void updateControls();
    // Used to add automation just before playing
    void updateAutomation(size_t index);
    void exportSequence();

    // Draws plot from player at size --> Used for both orginal and sampled audio
    void drawAudioPlayer(const Rectf& size, WaveformPlot plot, const BufferPlayerNodeRef& player);
    void drawSequencer();

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

    // For coloring borders and boxes
    Color border = Color( 0, 1, 0);
    Color currentColor = Color( 0, .5, .2);
    Color lightBlue = Color( 0, 0, 1);
    Color darkBlue = Color( 0, 0, .2);

    int numSamples = 0;
    // Useful constansts
    const int kSequenceLimit = 16;
    const float kWindowToWidgetRatio = float(8/7);
    const int kPaddedWidth = 700;
    const float kBoxSize = float(kPaddedWidth) / kSequenceLimit;
    const int kControlsYPosition = 550;

    // Tied to respective Cinder-UI sliders. Changes made to thes variables affect the current SampleController
    SuperCanvasRef mUi;
    SuperCanvasRef mUiControls;
    SuperCanvasRef mUiAutomation;
    float volume = 1;
    float pan = 0.5;
    int low = 1600;
    int high = 0;
    float finalVolume = 1;
    float finalPan = 0.5;



    // For the base sample that the user drags in
    BufferPlayerNodeRef mBufferPlayerNode;
    SourceFileRef mSourceFile;
    WaveformPlot mWaveformPlot;

    // For the clipped sample created from the original
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
