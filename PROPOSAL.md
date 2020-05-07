# Range Based Audio Sampler

Author: Sid Ajay

A Digital Audio Workstation (DAW) is an application that allows a user to manipulate audio and midi files to ultimately create music. 
The DAW can also host smaller “plugins”, which are tools which either create or manipulate audio. 
My orginal porject idea was to replicate a plugin called Vocalign, which can align two audio files by tracing over the guide audio then compressing and expanding the dub audio to fit the trace. 
After several days working with the AudioUnits library, I realized that this project is not feasible with the time and tools I currently have. With that being said, I am definetely glad a tried making the vocal alignment plugin. 
It is definetely a project left open to be finished in the future.
As a result, I have decided to create an Audio Sampler instead. 
The basic idea is for the user to load an audio sample, trim the sample to their desire, and then add effects to the region that they selected. This region can then be then be cloned 16 times,
where the user could modify the volume, pan, and EQ of each of the clones. The clones would the be strung together creating a more complicated sample. 


While I’ve wanted to build plugins like this for sometime, my biggest obstacle was a shaky understanding of C++, as it's used in most of the libraries and frameworks for creating plugins. Ultimately, my motivation to build this is to use it. 
Thus, my main goal and reason to see this project through is for the plugin to be able to interact with my DAW and help make music.



The library used for this project was a cinder-block called Cinder-UI. After fully disecting the AudioUnit library (my initial library), 
I have concluded that most of it's functionality is unusable due to deprecations and such. 
Therefore, I use Cinder-UI to create the sliders which control the level of volume, pan, and EQ on a given SampleController.


At the time I am writing this, I have about a week remaining. Currently I have the UI for trimming the sample and the on/off effects working for reverb and distortion. With the week I have remaining, I want to add more effect and presets to make the plugin more useful. Additionally, I want to delve further into controlling the sample
 through automation. I also want to clean up the UI and if I have time, integrate it with my DAW.


