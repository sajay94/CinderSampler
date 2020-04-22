// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>
#include "../resources/Resources.h"


namespace myapp {


using namespace ci;
using namespace ci::app;
using namespace std;

AudioAligner::AudioAligner() { }

void AudioAligner::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();

    auto ctx = audio::Context::master();

    // create a SourceFile and set its output samplerate to match the Context.
    audio::SourceFileRef sourceFile = audio::load( app::loadAsset("tasty burger.mp3"), ctx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = sourceFile->loadBuffer();
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
    }
}

void AudioAligner::fileDrop(FileDropEvent event) {
    fs::path filePath = event.getFile( 0 );
    getWindow()->setTitle( filePath.filename().string() );

    audio::SourceFileRef sourceFile = audio::load( loadFile( filePath ) );

    // BufferPlayerNode can also load a buffer directly from the SourceFile.
    // This is safe to call on a background thread, which would alleviate blocking the UI loop.
    mBufferPlayerNode->loadBuffer( sourceFile );

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

void AudioAligner::mouseDown(MouseEvent event) {
    mBufferPlayerNode->start();
}

void AudioAligner::mouseDrag(MouseEvent event) {
    if( mBufferPlayerNode->isEnabled() )
        mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

}  // namespace myapp
