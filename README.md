![travis-ci](https://travis-ci.org/dpayne/cli-visualizer.svg?branch=master)

# cli-visualizer

Command line visualizer. Currently only supports mpd.

This project was heavily inspired by [C.A.V.A](https://github.com/karlstav/cava), [ncmpcpp](http://rybczak.net/ncmpcpp/), and [rainbow](https://github.com/sickill/rainbow)

![spectrum_stereo](/examples/spectrum_stereo.gif?raw=true "Spectrum Stereo")

![spectrum_mono](/examples/spectrum_mono.gif?raw=true "Spectrum Mono")

![ellipse](/examples/ellipse.gif?raw=true "Ellipse")

![lorenz](/examples/lorenz.gif?raw=true "Lorenz")

**Table of Contents**

- [Installing Pre-requisites](#installing-pre-requisites)
 - [Ubuntu](#ubuntu)
 - [Arch Linux](#arch-linux)
 - [Mac OS X](#mac-os-x)
- [Installing](#installing)
- [MPD Setup](#mpd-setup)
- [ALSA Setup](#alsa-setup)
- [Pulse Audio Setup](#pulse-audio-setup)
- [Usage](#usage)
	- [Controls](#controls)
- [Configuration](#configuration)



## Installing Pre-requisites

`fftw` and `ncursesw` libraries are required to build. Note that ncurses with wide character support is needed.

A C++ compiler that supports C++14 is also needed. On arch linux, the latest g++ or clang++ will work.

### 256 Colors

In order to show the colors, you need a terminal with 256 color support.`rxvt-unicode` out of the box.

For xterm, the default `$TERM` setting needs to be changed to `xterm-256color`. To change this run

    export TERM=xterm-256color

### Ubuntu

    sudo apt-get install libfftw3-dev libncursesw5-dev
    
Older versions of Ubuntu also need newer a newer gcc compiler. Note, while this should be safe, it will upgrade some base libc libraries that might break your system.

	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install gcc-4.9 g++-4.9

### Arch Linux

In arch, the ncursesw is bundled with the ncurses package.

    sudo pacman -S ncurses fftw

### Mac OS X

Mac os x has a version of ncurses builtin, but a newer version is required.

	brew install fftw
	brew tap homebrew/dupes
	brew install ncurses

## Installing

After the pre-requisites have been installed, run the install script.

    ./install.sh

The configuration file is installed under "~/.vis/config".

Older version of Ubuntu need to compile with the newer g++ compiler.

	export CXX=g++-4.9
	./install.sh

### Arch Linux

The Arch Linux install is much simpler since an AUR package exist for cli-visualizer

	yaourt -S cli-visualizer

## MPD Setup
The visualizer needs to use mpd's fifo output file to read in the audio stream. To do this, add the following lines to your mpd config file.


	audio_output {
    	type                    "fifo"
    	name                    "my_fifo"
    	path                    "/tmp/mpd.fifo"
    	format                  "44100:16:2"
	}

If you have any sync issues with the audio where the visualizer either get ahead or behind the music, try lowering the audio buffer in your mpd config.

	audio_output {
        ...
        #this sets the buffer time to 50,000 microseconds
        buffer_time     "50000"
	}

## ALSA Setup
Similar to the MPD setup, the visualizer needs to use a fifo output file to read in the alsa audio stream. To do this, add the following lines to your alsa config file, usually at `/etc/asound.conf`. If the file does not exist create it under `/etc/asound.conf`. 


    pcm.!default {
        type file               # File PCM
        slave.pcm "hw:0,0"      # This should match the playback device at /proc/asound/devices
        file "|safe_fifo /tmp/audio" #safe_fifo will be in the cli-visualizer/bin/safe_fifo directory
        format raw              # File format ("raw" or "wav")
        perm 0666               # Output file permission (octal, def. 0600)
    }

A normal fifo file can not be used since otherwise no sound would work unless the fifo file is being read. This effectively means that no sound would be played if the visualizer is not running. To get around this issue a helper program `safe_fifo` is used. The `safe_fifo` program is essentially a non-blocking fifo file, it takes `stdin` and writes it to a fifo files given as the first parameter. If the fifo buffer is full, it clears the buffer and writes again.

Note that alsa support is still very experimental. There are a couple of caveats with this approach. Firstly `safe_fifo` must in a location alsa can find it. If you are building from source it us under `cli-visualizer/bin/safe_fifo`. Secondly `slave.pcm` must match whatever your alsa playback device is.

### ALSA with dmix
On sound cards that do not support hardware level mixing, alsa uses dmix to allow playback from multiple applications at once. In order to make this work with the visualizer a dmixer plugin needs to be defined in `/etc/asound.conf` and the visualizer pcm set to use `dmixer` instead of the hardware device directly. This configuration might will change slightly depending on the system. 

This is an example `asound.conf` for Intel HD Audio.

    pcm.dmixer { 
        type dmix 
        ipc_key 1024
        ipc_key_add_uid false
        ipc_perm 0666            # mixing for all users
        slave { 
            pcm "hw:0,0" 
            period_time 0 
            period_size 1024 
            buffer_size 8192
            rate 44100
        }
        bindings { 
            0 0 
            1 1 
        } 
    } 

    pcm.dsp0 { 
        type plug 
        slave.pcm "dmixer" 
    } 

    pcm.!default { 
        type plug 
        slave.pcm "dmixer" 
    } 

    pcm.default { 
       type plug 
       slave.pcm "dmixer" 
    } 

    ctl.mixer0 { 
        type hw 
        card 0 
    }

    pcm.!default {
        type file               # File PCM
        slave.pcm "dmixer"      # Use the dmixer plugin as the slave pcm
        file "|safe_fifo /tmp/audio"
        format raw              # File format ("raw" or "wav")
        perm 0666               # Output file permission (octal, def. 0600)
    }

## Pulse Audio Setup
Currently I have not been able to test a pulseaudio setup since my current computer does not work well with pulseaudio. Theoretically the setup should be the same as the alsa setup but I have not tested this at all.

## Usage

Start with

	vis

###Controls

| Key | Description |
| --- | ----------- |
| <kbd>space</kbd> | switch visualizers |
| <kbd>q</kbd> or <kbd>CTRL-C</kbd>| Quit |

## Configuration
    #Refresh rate of the visualizers. A really high refresh rate may cause screen tearing. Default is 20.
    visualizer.fps=20

    #Defaults to "/tmp/mpd.fifo"
    mpd.fifo.path

    #If set to false the visualizers will use mono mode instead of stereo. Some visualizers will
    #behave differently when mono is enabled. For example, spectrum show two sets of bars.
    audio.stereo.enabled=false

    #Specifies how often the visualizer will change in seconds. 0 means do not rotate. Default is 0.
    visualizer.rotation.secs=10

    #Configures the samples rate and the cutoff frequencies.
    audio.sampling.frequency=44100
    audio.low.cutoff.frequency=22050
    audio.high.cutoff.frequency=30


    #Configures the visualizers and the order they are in. Available visualizers are spectrum,lorenz,ellipse.
    #Defaults to spectrum,ellipse,lorenz
    visualizers=spectrum,ellipse,lorenz


    #Configures what character the spectrum visualizer will use. Specifying a space (e.g " ") means the
    #background will be colored instead of the character. Defaults to " ".
    visualizer.spectrum.character=#

    #Spectrum bar width. Defaults to 1.
    visualizer.spectrum.bar.width=2

    #The amount of space between each bar in the spectrum visualizer. Defaults to 1. It's possible to set this to
    #zero to have no space between bars
    visualizer.spectrum.bar.spacing=1

    #Available smoothing options are monstercat, sgs, none.
    visualizer.spectrum.smoothing.mode=monstercat

    #This configures the falloff effect on the spectrum visualizer. Available falloff options are fill,top,none.
    #Defaults to "fill"
    visualizer.spectrum.falloff.mode=fill

    #Configures how fast the falloff character falls. This is an exponential falloff so values usually look
    #best 0.9+ and small changes in this value can have a large effect. Defaults to 0.99
    visualizer.spectrum.falloff.weight=0.99

    #This configures the sgs smoothing effect on the spectrum visualizer. More points spreads out the smoothing
    #effect and increasing passes runs the smoother multiple times on reach run. Defaults are points=3 and passes=1
    visualizer.sgs.smoothing.points=3
    visualizer.sgs.smoothing.passes=1


    #Configures what character the ellipse visualizer will use. Specifying a space (e.g " ") means the
    #background will be colored instead of the character. Defaults to " ".
    visualizer.ellipse.character=#

    #The radius of each color ring in the ellipse visualizer. Defaults to 2.
    visualizer.ellipse.radius=2

    #Specifies the color scheme. The color scheme must be in ~/.vis/colors/ directory. Default is "colors"
    colors.scheme=rainbow
