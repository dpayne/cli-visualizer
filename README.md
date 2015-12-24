# cli-visualizer

Command line visualizer. Currently only supports mpd.

This project was heavily inspired by [C.A.V.A](https://github.com/karlstav/cava), [ncmpcpp](http://rybczak.net/ncmpcpp/), and [rainbow](https://github.com/sickill/rainbow)


![spectrum_stereo](/examples/spectrum_stereo.gif?raw=true "Spectrum Stereo")

![spectrum_mono](/examples/spectrum_mono.gif?raw=true "Spectrum Mono")

![ellipse](/examples/ellipse.gif?raw=true "Ellipse")

![lorenz](/examples/lorenz.gif?raw=true "Lorenz")

## Installing Pre-requisites

`fftw` and `ncursesw` libraries are required to build. Note that ncurses with wide character support is needed.

An C++ compiler that supports C++14 is also needed. On arch linux, the latest g++ or clang++ will work.

### Ubuntu

    sudo apt-get install libfftw3-dev libncursesw5-dev

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
