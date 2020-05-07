
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
        mGain->setValue(volume);

        mPan = ctx->makeNode( new audio::Pan2dNode );
        mPan->setPos(pan);


        mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );
        mLowPass->setCutoffFreq(low);

        mHighPass = ctx->makeNode( new audio::FilterHighPassNode() );
        mHighPass->setCutoffFreq(high);

        mBufferPlayerNode >> mGain >> mPan >> mLowPass >> mHighPass >> ctx->getOutput();
        ctx->enable();

    }

    BufferPlayerNodeRef SampleController::getSampleBufferPlayer() {
        return mBufferPlayerNode;
    }

    float SampleController::getVolume() {
        return volume;
    }

    void SampleController::setVolume(float value) {
        volume = value;
        mGain->setValue(volume);
    }

    float SampleController::getPan() {
        return pan;
    }

    void SampleController::setPan(float value) {
        pan = value;
        mPan->setPos(value);
    }

    float SampleController::getLowPass() {
        return low;
    }

    void SampleController::setLowPass(int freq) {
        low = freq;
        mLowPass->setCutoffFreq(freq);
    }

    float SampleController::getHighPass() {
        return mHighPass->getCutoffFreq();
    }

    void SampleController::setHighPass(int freq) {
        mHighPass->setCutoffFreq(freq);
    }

    float SampleController::getParamVolume() {
        return finalVolume;
    }

    void SampleController::setParamVolume(float value) {
        finalVolume = value;
    }

    void SampleController::setRampVolume() {
        auto options = audio::Param::Options().rampFn( &audio::rampOutQuad );
        mGain->getParam()->reset();
        mGain->getParam()->applyRamp(volume, finalVolume, 1/mBufferPlayerNode->getLoopEndTime());
    }

    float SampleController::getParamPan() {
        return finalPan;
    }

    void SampleController::setParamPan(float value) {
        finalPan = value;
    }

    void SampleController::setRampPan() {
        auto options = audio::Param::Options().rampFn( &audio::rampOutQuad );
        mPan->getParamPos()->reset();
        mPan->getParamPos()->applyRamp(pan, finalPan, 1/mBufferPlayerNode->getLoopEndTime());
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