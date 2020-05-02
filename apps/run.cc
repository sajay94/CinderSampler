// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

#include "my_app.h"

#include "../../../samples/_audio/BufferPlayer/include/Resources.h"
#include "../../../samples/_audio/common/AudioDrawUtils.h"


using cinder::app::App;
using cinder::app::RendererGl;


namespace myapp {

const int kSamples = 8;
const int kWidth = 800;
const int kHeight = 700;

void SetUp(App::Settings* settings) {
  settings->setWindowSize(kWidth, kHeight);
    settings->setResizable(false);
  settings->setTitle("Range Based Audio Sampler");
}

}  // namespace myapp


// This is a macro that runs the application.
CINDER_APP(myapp::AudioAligner,
           RendererGl(RendererGl::Options().msaa(myapp::kSamples)),
           myapp::SetUp)
