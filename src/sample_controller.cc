
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
    using namespace reza::ui;

    SampleController::SampleController() {

    }

    void SampleController::setup(const fs::path asset) {
        auto ctx = audio::Context::master();
        mSourceFile = audio::load( app::loadAsset(asset), ctx->getSampleRate() );

        audio::BufferRef buffer = mSourceFile->loadBuffer();
        mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );

        mGain = ctx->makeNode( new audio::GainNode );
        mGain->setValue(1.0);

        mPan = ctx->makeNode( new audio::Pan2dNode );
        mPan->setPos(0);

        mDelay = ctx->makeNode( new audio::DelayNode );
        mDelay->setDelaySeconds(0  );

        mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );
        mLowPass->setCutoffFreq(1600);

        mHighPass = ctx->makeNode( new audio::FilterHighPassNode() );
        mHighPass->setCutoffFreq(0);

        mBufferPlayerNode >> mGain >> mPan >> mLowPass >> mHighPass >> ctx->getOutput();
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

    BufferRecorderNodeRef SampleController::getBufferRecorderNode() {
        return mRecorderNode ;
    }

    WaveformPlot SampleController::getSampleWaveformPlot() {
        return mWaveformPlot;
    }

    SourceFileRef SampleController::getSampleSourceFile() {
        return mSourceFile;
    }

    float SampleController::getVolume() {
        return mGain->getValue();
    }

    void SampleController::setVolume(float value) {
        mGain->setValue(value);
    }

    float SampleController::getPan() {
        return mPan->getPos();
    }

    void SampleController::setPan(float value) {
        mPan->setPos(value);
    }

    float SampleController::getLowPass() {
        return mLowPass->getCutoffFreq();
    }

    void SampleController::setLowPass(int freq) {
        mLowPass->setCutoffFreq(freq);
    }

    float SampleController::getHighPass() {
        return mHighPass->getCutoffFreq();
    }

    void SampleController::setHighPass(int freq) {
        mHighPass->setCutoffFreq(freq);
    }

    void SampleController::writeToFile(const fs::path asset) {
        auto ctx = mBufferPlayerNode->getContext();
        mRecorderNode = ctx->makeNode( new audio::BufferRecorderNode);
        mRecorderNode->setNumFrames(mBufferPlayerNode->getNumFrames(), true);
        mBufferPlayerNode >> mGain >> mPan >> mRecorderNode;
        mRecorderNode->start();
        mRecorderNode->writeToFile(asset);
    }
}