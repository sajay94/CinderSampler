# Range Based Audio Sampler

![App-Demo](https://user-images.githubusercontent.com/55111487/81249083-cac1b380-8feb-11ea-9da7-37a2329fb028.png)

A Range Based Audio Sampler allows the user to select and audio file, clip it to 
a desired length, and then sample the clip. Sampling allows the clip to be repeated some n times,
where the user can specify controls such as volume, pan, and EQ on each of the n samples individually.
In effect, the user should be able to use the sample as a base to create more complicated samples.

See PROPOSAL.md for more details

## Dependencies
 [Cinder](https://libcinder.org/download) - Basic Framework used to create app. Used cinder::audio to
 handle all audio files and effects
 
 [Cmake](https://cmake.org/download/) - Build System
 
 [Cinder-UI](https://github.com/rezaali/Cinder-UI) - UI cinder block. Used to create UI for sample controls. To build,
 follow directions on link and block to project.
 
 [Catch-2](https://github.com/catchorg/Catch2) - Used for testing


## Controls
  Mouse - Use mouse to interact with AudioBounds and Cinder-UI sliders.
  
  SpaceBar - Start/Stops the most recent audioBuffer the user clipped.
  
  'c' - clips the top most audioBuffer at the region specified by the AudioBounds.
  
  's' - pressed when a sample is ready to be placed in Sequencer. Only possible after 'c' atleast once.
  
  'p' - play the sequence -> only possible after 's'
    
 
