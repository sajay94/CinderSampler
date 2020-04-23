// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

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


AudioAligner::AudioAligner() { }

void AudioAligner::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();

    auto ctx = audio::Context::master();

    //AudioUnit demo
    reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    speechSynth.connectTo(reverb).connectTo(tap).connectTo(mixer).connectTo(output);
    output.start();

    // create a SourceFile and set its output samplerate to match the Context.
    mSourceFile = audio::load( app::loadAsset("tasty burger.mp3"), ctx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = mSourceFile->loadBuffer();
    mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );

    // add a Gain to reduce the volume
    mGain = ctx->makeNode( new audio::GainNode( 0.5f ) );

    // connect and enable the Context
    mBufferPlayerNode >> mGain >> ctx->getOutput();
    ctx->enable();
}

void AudioAligner::draw() {
    gl::clear();
    gl::enableAlphaBlending();

    mWaveformPlot.draw();

    // draw the current play position
    float readPos = (float)getWindowWidth() * mBufferPlayerNode->getReadPosition() / mBufferPlayerNode->getNumFrames();
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, (float)getWindowHeight() ) );
}

void AudioAligner::keyDown(KeyEvent event) {
    if( event.getCode() == KeyEvent::KEY_SPACE ) {
        if( mBufferPlayerNode->isEnabled() )
            mBufferPlayerNode->stop();
        else
            mBufferPlayerNode->start();
    } else if( event.getCode() == KeyEvent::KEY_w )
        this->exportFile();
}

void AudioAligner::fileDrop(FileDropEvent event) {
    fs::path filePath = event.getFile( 0 );
    getWindow()->setTitle( filePath.filename().string() );

    mSourceFile.reset();
    mSourceFile = audio::load( loadFile( filePath ) );

    // BufferPlayerNode can also load a buffer directly from the SourceFile.
    // This is safe to call on a background thread, which would alleviate blocking the UI loop.
    mBufferPlayerNode->loadBuffer( mSourceFile );

    mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
}

void AudioAligner::resize() {
    // visualize the audio buffer
    ci::Area bounds = getWindowBounds();
    bounds.setY1(bounds.getY1() - bounds.getHeight()/2);
    bounds.setY2(bounds.getY2() - bounds.getHeight()/2);

    if( mBufferPlayerNode )
        mWaveformPlot.load( mBufferPlayerNode->getBuffer(), bounds);
}
void AudioAligner::exportFile() {
    audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

    try {
        const fs::path fileName = "/Users/family/Cinder/Projects/final-project-sajay94/assets/out2.wav";
        audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels() ); // INT_16
        //	audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels(), audio::SampleType::FLOAT_32 );

        target->write( audioBuffer.get() );

        }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
}

void AudioAligner::mouseDown(MouseEvent event) {
    //mBufferPlayerNode->start();
    std::string hello("Hello!");
    speechSynth.say(hello);
}

void AudioAligner::mouseDrag(MouseEvent event) {
    if( mBufferPlayerNode->isEnabled() )
        mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

}  // namespace myapp
