
#include "mylibrary/sample_controller.h"

#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>


#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"



namespace myapp {

    using namespace ci;
    using namespace ci::app;
    using namespace std;

    SampleController::SampleController() {

    }

    void SampleController::setup(fs::path asset) {
        auto ctx = audio::Context::master();
        mSourceFile = audio::load( app::loadAsset("out0.wav"), ctx->getSampleRate() );

        audio::BufferRef buffer = mSourceFile->loadBuffer();
        mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );

        // audio::Node method -- works
        mDelay = ctx->makeNode( new audio::DelayNode );
        mDelay->setDelaySeconds(3  );

        mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );
        mLowPass->setCutoffFreq( 400 );

        mBufferPlayerNode >> ctx->getOutput();
        ctx->enable();

//    audio::cocoa::EffectNode method
//    delay = ctx->makeNode( new audio::cocoa::EffectAudioUnitNode( kAudioUnitSubType_Delay ) );
//    delay->setParameter(kDelayParam_DelayTime, 1);
//    lowpass = ctx->makeNode( new audio::cocoa::EffectAudioUnitNode( kAudioUnitSubType_LowPassFilter ) );
//    lowpass->setParameter( kLowPassParam_CutoffFrequency, 500 );
    }

    BufferPlayerNodeRef SampleController::getSampleBufferPlayer() {
        return mBufferPlayerNode;
    }

    WaveformPlot SampleController::getSampleWaveformPlot() {
        return mWaveformPlot;
    }
}