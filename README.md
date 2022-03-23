<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [cli-visualizer](#cli-visualizer)
  - [Installing Pre-requisites](#installing-pre-requisites)
    - [256 Colors](#256-colors)
    - [Debian & Ubuntu](#debian--ubuntu)
    - [Arch Linux](#arch-linux)
    - [Fedora](#fedora)
    - [Solus](#solus)
    - [Gentoo](#gentoo)
    - [Mac OS X](#mac-os-x)
  - [Installing](#installing)
    - [Arch Linux](#arch-linux-1)
  - [Setup](#setup)
    - [MPD Setup](#mpd-setup)
    - [ALSA Setup](#alsa-setup)
      - [ALSA with dmix](#alsa-with-dmix)
    - [Pulse Audio Setup (Easy)](#pulse-audio-setup-easy)
    - [Port Audio Setup (Wicked Easy)](#port-audio-setup-wicked-easy)
    - [squeezelite Setup](#squeezelite-setup)
  - [Usage](#usage)
    - [Controls](#controls)
  - [Configuration](#configuration)
    - [Reloading Config](#reloading-config)
    - [Colors](#colors)
      - [Color Schemes by Visualizer](#color-schemes-by-visualizer)
      - [Color Gradients](#color-gradients)
      - [RGB colors](#rgb-colors)
      - [Color indexes](#color-indexes)
      - [Color names](#color-names)
    - [Spectrum](#spectrum)
      - [Smoothing](#smoothing)
        - [Sgs Smoothing](#sgs-smoothing)
        - [MonsterCat Smoothing](#monstercat-smoothing)
        - [No Smoothing](#no-smoothing)
      - [Falloff](#falloff)
      - [Spectrum Appearance](#spectrum-appearance)
    - [Scaling](#scaling)
    - [Full configuration example](#full-configuration-example)
  - [Trouble Shooting](#trouble-shooting)
    - [General](#general)
      - [vis is overwriting my terminal colorscheme](#vis-is-overwriting-my-terminal-colorscheme)
      - [Tearing our corrupt output](#tearing-our-corrupt-output)
    - [Mac OSX](#mac-osx)
      - [vis hangs with no output #1](#vis-hangs-with-no-output-1)
      - [vis hangs with no output #2](#vis-hangs-with-no-output-2)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

![travis-ci](https://travis-ci.org/dpayne/cli-visualizer.svg?branch=master)

![Coverity Scan Build Status](https://scan.coverity.com/projects/7519/badge.svg)

# cli-visualizer

Command line visualizer. Supports mpd, with experimental support for alsa and pulseaudio.

This project was heavily inspired by [C.A.V.A](https://github.com/karlstav/cava), [ncmpcpp](http://rybczak.net/ncmpcpp/), and [rainbow](https://github.com/sickill/rainbow)

![spectrum_stereo](https://i.imgur.com/BGyfYiv.gif "Spectrum Stereo")

![spectrum_mono](https://i.imgur.com/Zo5lByM.gif "Spectrum Mono")

![ellipse](https://i.imgur.com/WoQlObi.gif "Ellipse")

![lorenz](https://i.imgur.com/9QJjnDI.gif "Lorenz")


## Installing Pre-requisites

`fftw` and `ncursesw` libraries are required to build. Note that ncurses with wide character support is needed.

A C++ compiler that supports C++14 is also needed. On arch linux, the latest g++ or clang++ will work.

### 256 Colors

In order to show the colors, you need a terminal with 256 color support.`rxvt-unicode` out of the box.

For xterm, the default `$TERM` setting needs to be changed to `rxvt-256color`. To change this run

    export TERM=rxvt-256color

Setting TERM to `xterm-256color` may also work but seems to cause issues with rendering of the visualizers in some terminals.

### Debian & Ubuntu

	sudo apt install libfftw3-dev libncursesw5-dev cmake

For pulseaudio support, the pulseaudio library also needs to be installed

	sudo apt install libpulse-dev

Older versions of Ubuntu also need newer a newer gcc compiler. Note, while this should be safe, it will upgrade some base libc libraries that might break your system.

	sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install gcc-4.9 g++-4.9

### Arch Linux

In arch, the ncursesw is bundled with the ncurses package.

    sudo pacman -S ncurses fftw cmake

### Fedora

	sudo dnf install fftw-devel ncurses-devel pulseaudio-libs-devel cmake

### Solus

Solus requires a handful of development packages and creating a link for libtinfo before installing.

	sudo eopkg it -c system.devel
	sudo eopkg install fftw fftw-devel ncurses ncurses-devel pulseaudio-devel cmake
	sudo ln -s /usr/lib/libncurses.so.5 /usr/lib/libtinfo.so.5 # Should already be there, but just in case.
	sudo ln -s /usr/lib/libtinfo.so.5 /usr/lib/libtinfo.so

	./install.sh

### Gentoo

Make sure to compile `ncurses` with `unicode` flag, have `fftw` and
`cmake` installed on your Gentoo system.

	sudo emerge fftw cmake
	sudo USE="unicode" emerge ncurses

### Mac OS X

Mac os x has a version of ncurses builtin, but a newer version is required.

	brew install fftw cmake
	brew tap homebrew/dupes
	brew install ncurses

## Installing

After the pre-requisites have been installed, run the install script.

    ./install.sh

The configuration file is installed under "~/.config/vis/config".

Older version of Ubuntu need to compile with the newer g++ compiler.

	export CXX=g++-4.9
	./install.sh

### Arch Linux

The Arch Linux install is much simpler since an AUR package exist for cli-visualizer.

	yaourt -S cli-visualizer

You will have to copy config and colors manually.

## Setup

At least one of the following needs to be configured (and linked in the config): mpd, alsa, pulseaudio or squeezelite.

### MPD Setup

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

### ALSA Setup

WARNING: alsa support is very very experimental. It is very possible that it will be completely broken on various systems and could cause weird/bad behaviour.

Similar to the MPD setup, the visualizer needs to use a fifo output file to read in the alsa audio stream. To do this, add the following lines to your alsa config file, usually at `/etc/asound.conf`. If the file does not exist create it under `/etc/asound.conf`.

    pcm.!default {
        type file               # File PCM
        slave.pcm "hw:0,0"      # This should match the playback device at /proc/asound/devices
        file "|safe_fifo /tmp/audio" #safe_fifo will be in the cli-visualizer/bin/safe_fifo directory
        format raw              # File format ("raw" or "wav")
        perm 0666               # Output file permission (octal, def. 0600)
    }

Next change the visualizer config `~/.config/vis/config` to point to the new fifo file

    mpd.fifo.path=/tmp/audio

A normal fifo file can not be used since otherwise no sound would work unless the fifo file is being read. This effectively means that no sound would be played if the visualizer is not running. To get around this issue a helper program `safe_fifo` is used. The `safe_fifo` program is essentially a non-blocking fifo file, it takes `stdin` and writes it to a fifo files given as the first parameter. If the fifo buffer is full, it clears the buffer and writes again.

Note that alsa support is still very experimental. There are a couple of caveats with this approach. Firstly `safe_fifo` must in a location alsa can find it. If you are building from source it us under `cli-visualizer/bin/safe_fifo`. Secondly `slave.pcm` must match whatever your alsa playback device is.

#### ALSA with dmix

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

### Pulse Audio Setup (Easy)

Pulse audio should be the easiest to setup out of all the options. In order for this to work pulseaudio must be installed and vis must be built with pulseaudio enabled. To build with pulseaudio support run `make ENABLE_PULSE=1`.

To enable pulse audio in the vis config set audio sources to pulse with

    audio.sources=pulse

If this does not work, then vis has most likely not guess the correct sink to use. Try switching the pulseaudio source vis uses. A list can be found by running the command `pacmd list-sinks  | grep -e 'name:'  -e 'index'`. The correct pulseaudio source can then be set with

    audio.pulse.source=0

**Troubleshooting with Pulse For MacOS Users:**

If vis is hanging for you after running it make sure the pulseaudio daemon is running. You can start it if you installed it with brew by running

    brew services start pulseaudio

Alternatively you can run the `pulseaudio` command and send it to the background, then run `bg` to continue it.

Then try to run `vis` again. You'll know it worked if it asks to have access to your microphone.



### Port Audio Setup (Wicked Easy)

Install portaudio, then build vis. That's it.

To enable port audio in the vis config set audio sources to port with

    audio.sources=port

The correct port audio device can then be set with

    audio.port.source=DEVICE NAME

### squeezelite Setup
[squeezelite](https://en.wikipedia.org/wiki/Squeezelite) is one of several software clients available for the Logitech Media Server. Squeezelite can export its audio data as shared memory, which is what this source module uses. Just adapt your config:

	audio.sources=shmem
	shmem.shmemname=/squeezelite-AA:BB:CC:DD:EE:FF

where AA:BB:CC:DD:EE:FF is squeezelite's MAC address (check the LMS Web GUI (Settings&gt;Information) if unsure).  
Note: squeezelite must be started with the `-v` flag to enable visualizer support.

## Usage

Start with

	vis

### Controls

| Key | Description |
| --- | ----------- |
| <kbd>space</kbd> | Switch visualizers |
| <kbd>q</kbd> or <kbd>CTRL-C</kbd>| Quit |
| <kbd>r</kbd>| Reload config |
| <kbd>c</kbd>| Next color scheme |
| <kbd>s</kbd>| Toggle Mono/Stereo Mode |
| <kbd>+</kbd>| Increase scaling by 10% |
| <kbd>-</kbd>| Decrease scaling by 10% |

## Configuration

### Reloading Config

The config can be reload while `vis` is running by either pressing the `r` key or by sending the `USR1` signal to `vis`. Sending the `USR1` signal can be done with `killall -USR1 vis`, this is useful if you want to dynamically reload colors from a script.

### Colors

The display colors and their order can be changed by switching the color scheme in the config under `colors.scheme`. This is a comma separated list of color schemes.
Each color scheme must be defined at `~/.config/vis/colors/<name_of_the_color_scheme>`. There are three different ways to specific a color within the scheme: by name, by hex number, and by index.

An example color scheme configuration with only one scheme would be

    colors.scheme=rainbow

Using two color schemes

    colors.scheme=rainbow,blue

Note, vis does not override or change any of the terminal colors. All colors will be influenced by whatever terminal settings are set by the users terminal. Usually these colors are specified in `.Xdefaults`.

#### Color Schemes by Visualizer

Each visualizer supports its own list of color schemes. This will control the colors of a particular visualizer without effecting the others and will override the global defined by `colors.scheme`.

An example of settings the spectrums visualizer to rainbow color scheme and the default for other visualizers to the blue color scheme.

    visualizer.spectrum.color.scheme=rainbow
    color.scheme=blue

#### Color Gradients

Gradients are turned on by default. To disable gradients add `gradient=false` to the top of the color scheme file. Note this is NOT in the main config but instead in the color scheme file.

Color scheme example with gradients disabled.

    gradient=false
    #4040ff
    #03d2aa
    #56fc2d

Color scheme example with gradients enabled that blends red to blue.

    red
    blue

#### RGB colors

The color scheme can be defined in two ways with RGB values or by index. The RGB values are defined as a hex color code, they are useful for creating a 256 bit color scheme.
The displayed color will not be the true color, but instead an approximation of the color based on 256-bit terminal colors.

RGB color scheme example

    #4040ff
    #2c56fc
    #2a59fc
    #1180ed
    #04a6d5
    #02abd1
    #03d2aa
    #04d6a5
    #12ee7f
    #2bfc58
    #2dfc55
    #56fc2d

#### Color indexes

The second way to define a color scheme is by the color index. Specifically this is the exact color index used by ncurses.
Color indexes are useful for creating a visualizer that matches the terminal's set color scheme.

Color index color scheme example

    4
    12
    6
    14
    2
    10
    11
    3
    5
    1
    13
    9
    7
    15
    0


![basic_colors](https://web.archive.org/web/20160228121439if_/https://camo.githubusercontent.com/0c48ed2b72622c935165c5127cb2cc699f46ea0c/687474703a2f2f692e696d6775722e636f6d2f545572515530316c2e676966 "Basic Colors")

All the basic 16 terminal colors from 0-15 in-order. The spectrum colors can be ordered in any you want, this example was done in order to show all colors.

<br><br>


#### Color names

The third way to define a color scheme is by the color name. Only 8 color names are supported: black, blue, cyan, green, yellow, red, magenta, white.
Note that these 8 colors are the basic terminal colors and are often set by a terminal's color scheme. This means that the color name might not match the color shown since the terminal theme might change it.
For example, dark themes often set `white` to something dark since `white` is usually the default color for the terminal background.

![blue](https://web.archive.org/web/20160228121442if_/https://camo.githubusercontent.com/31f5dda569d6bfa2ef50f126773172f465ee5315/687474703a2f2f692e696d6775722e636f6d2f766e31753974566c2e676966 "Blue")

A color scheme with only one color `blue`.

<br><br>


### Spectrum

The spectrum visualizer allows for many different configuration options.

#### Smoothing

There are three different smoothing modes, monstercat, sgs, none.

    #Available smoothing options are monstercat, sgs, none.
    visualizer.spectrum.smoothing.mode=sgs

##### Sgs Smoothing

SGS smoothing [Savitzky-Golay filter](https://en.wikipedia.org/wiki/Savitzky%E2%80%93Golay_filter). There are a couple of options for sgs smoothing.

The first option is controlling the number of smoothing passes done when rendering the bars. Increasing this number with make the spectrum smoother. This is set with `visualizer.sgs.smoothing.passes`.

The second option is the number of neighbors to look at when smoothing. This is set with `visualizer.sgs.smoothing.points`. This should always be an odd number.
A larger number will generally increase the smoothing since more neighbors will be looked at.

![sgs_smoothing](https://web.archive.org/web/20160228121446if_/https://camo.githubusercontent.com/9e44a48c1054b7811e2d8c2361ecb50899928088/687474703a2f2f692e696d6775722e636f6d2f667578754e51546c2e676966 "Sgs Smoothing")

Default sgs smoothing.

<br><br>


![sgs_smoothing](https://web.archive.org/web/20160228121449if_/https://camo.githubusercontent.com/b5c60fd86580c295672aa6ce43f665ca81f229f9/687474703a2f2f692e696d6775722e636f6d2f707446305568426c2e676966 "Sgs Smoothing High Pass")

Sgs smoothing with number of passes set to `5`.

##### MonsterCat Smoothing

Monster cat smoothing is inspired by the monster cat youtube channel (https://www.youtube.com/user/MonstercatMedia). To control the amount of smoothing for montercat use `visualizer.monstercat.smoothing.factor`. The default smoothing factor for monstercat is `1.5`. Increase the smoothing factor by a lot could hurt performance.

![monstercat_smoothing](https://web.archive.org/web/20160228121453if_/https://camo.githubusercontent.com/9a753ed6d780255f13e045af06345b84d0ec8689/687474703a2f2f692e696d6775722e636f6d2f31776d67726a656c2e676966 "MonsterCat")


##### No Smoothing

Smoothing can be completely turned off by setting the smoothing option to `none`.

    visualizer.spectrum.smoothing.mode=none

Spectrum with smoothing off

![none_smoothing](https://web.archive.org/web/20160228121452if_/https://camo.githubusercontent.com/56b0cedb7696c9bb1adf092ca2be616182e66575/687474703a2f2f692e696d6775722e636f6d2f38764c5339316b6c2e676966 "No Smoothing")


#### Falloff

This configures the falloff effect on the spectrum visualizer. This effect creates a slow fall in bar height. Available falloff options are `fill,top,none`. The default falloff option is `fill`.

    visualizer.spectrum.falloff.mode=fill

With the `top` setting the falloff effect is only applied to the top character in the bar. This creates a gap between the main bar and the top falloff character.

![top_falloff](https://web.archive.org/web/20160228121459if_/https://camo.githubusercontent.com/feffa089f19172af420596c0c86c8369d515e335/687474703a2f2f692e696d6775722e636f6d2f4636344e4257592e676966 "Top falloff")

top falloff effect with the spectrum character set to `#`.
<br><br>

The `fill` option leaves no gaps between the very top character and the rest of the bar.


![fill_falloff](https://web.archive.org/web/20160228121500if_/https://camo.githubusercontent.com/25787201d24335811997b57edcfcad5987b373e9/687474703a2f2f692e696d6775722e636f6d2f52554f556b4b512e676966 "Fill falloff")

fill falloff effect with the spectrum character set to `#`.

<br><br>

The `none` option removing the falloff effect entirely..

![none_falloff](https://web.archive.org/web/20160228121620if_/https://camo.githubusercontent.com/2bd05e48f7ea07bbe26b522abc7263c06953f69f/687474703a2f2f692e696d6775722e636f6d2f445850516470322e676966 "No Falloff")

Falloff effect removed with the spectrum character set to `#`.

#### Spectrum Appearance

Certain aspects of the spectrum appearance can be controlled through the config.

The bar width can be controlled to make it wider or narrower. The default is `2`.

    visualizer.spectrum.bar.width=2

![bar_width](https://web.archive.org/web/20160228121621if_/https://camo.githubusercontent.com/70c02d20e43cc7dec4804dc6c91bde40764aa542/687474703a2f2f692e696d6775722e636f6d2f4c5435775247566c2e676966 "Wide bar width")

Bar width set to `5`.

<br><br>

The spacing between bars can also be controlled to make it wider or narrower. The default is `1`.

    visualizer.spectrum.bar.spacing=1

![bar_spacing](https://web.archive.org/web/20160228121623if_/https://camo.githubusercontent.com/9303a29b961d222dbd21860ea479335a2aef7e7c/687474703a2f2f692e696d6775722e636f6d2f564f386f485a426c2e676966 "No bar spacing")

Bar spacing set to `0`.

<br><br>

The margin widths of the spectrum visualizer can also be controlled. The margins are set in percent of total screen for spectrum visualizer. All margin percentages default to `0.0`.

    visualizer.spectrum.top.margin=0.0
    visualizer.spectrum.bottom.margin=0.0
    visualizer.spectrum.right.margin=0.0
    visualizer.spectrum.left.margin=0.0

![spectrum_margins](http://i.imgur.com/t1t3cdjl.gif "Spectrum Margins")

Spectrum with the top margin set to `0.30`, stereo mode set to false, and the left right margins set to `0.10`.

<br>

Turn stereo mode off to show only one set of bars.

    audio.stereo.enabled=false

<br><br>

Normally the bars are ordered so that the lowest frequencies are handled by the left most bars and the highest frequencies by the right most bars.
The reversed option gives the option to reverse this so that the highest frequencies come first.

    visualizer.spectrum.reversed=true

![spectrum_reversed](http://i.imgur.com/EDhaRDrl.gif "Reversed")

Spectrum with reverse set to `true`.

### Scaling

The spectrum visualizer should auto-scale, but for lorenz and ellipse visualizers scaling is based off volume. If your music volume is a lot lower than the system volume then you can scale the visualizers with the settings

    visualizer.scaling.multiplier=1.5

This setting can also be controlled with `+/-` keys.

### Full configuration example

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
    audio.low.cutoff.frequency=30
    audio.high.cutoff.frequency=22050

    ##Applies scaling factor to both lorenz and ellipse visualizers. This is useful when the system audio is set
    #to a low volume.
    #visualizer.scaling.multiplier=1.0

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

    #Available smoothing options are monstercat, sgs, none. Defaults to sgs.
    visualizer.spectrum.smoothing.mode=monstercat

    #This configures the falloff effect on the spectrum visualizer. Available falloff options are fill,top,none.
    #Defaults to "fill"
    visualizer.spectrum.falloff.mode=fill

    #Configures how fast the falloff character falls. This is an exponential falloff so values usually look
    #best 0.9+ and small changes in this value can have a large effect. Defaults to 0.95
    visualizer.spectrum.falloff.weight=0.95

    #Margins in percent of total screen for spectrum visualizer. All margins default to 0
    visualizer.spectrum.top.margin=0.0
    visualizer.spectrum.bottom.margin=0.0
    visualizer.spectrum.right.margin=0.0
    visualizer.spectrum.left.margin=0.0

    #Reverses the direction of the spectrum so that high freqs are first and low freqs last. Defaults to false.
    visualizer.spectrum.reversed=false

    #Sets the audio sources to use. Currently available ones are "mpd" and "alsa"Sets the audio sources to use.
    #Currently available ones are "mpd", "pulse", "port", and "alsa". Defaults to "mpd".
    audio.sources=pulse

    ##vis tries to find the correct pulseaudio sink, however this will not work on all systems.
    #If pulse audio is not working with vis try switching the audio source. A list can be found by running the
    #command pacmd list-sinks  | grep -e 'name:'  -e 'index'
    audio.pulse.source=0

    #vis tries to find the correct portaudio device (through portaudio default method), however its not for-sure.
    #If port audio is not working with vis try switching the audio srouce. A list can be found by setting
    #this value to "list" and checking the vis log file. Replace this value with the device name desired
    audio.port.source=auto

    #This configures the sgs smoothing effect on the spectrum visualizer. More points spreads out the smoothing
    #effect and increasing passes runs the smoother multiple times on reach run. Defaults are points=3 and passes=2.
    visualizer.sgs.smoothing.points=3
    visualizer.sgs.smoothing.passes=2


    #Configures what character the ellipse visualizer will use. Specifying a space (e.g " ") means the
    #background will be colored instead of the character. Defaults to " ".
    visualizer.ellipse.character=#

    #The radius of each color ring in the ellipse visualizer. Defaults to 2.
    visualizer.ellipse.radius=2

    ## Turns off overriding the user's terminal colors
    #colors.override.terminal=false

    #Specifies the color scheme. The color scheme must be in ~/.config/vis/colors/ directory. Default is "colors"
    colors.scheme=rainbow

## Trouble Shooting

### General

#### vis is overwriting my terminal colorscheme

Turn off overriding terminal colors.

    colors.override.terminal=false

Due to the way terminal color schemes work, there is no portable way for ncurses to get the original color scheme. By default it overrides the user's terminal colors with the visualizer colors. It does this to get a smoother transition between gradients, however this overrides the user's terminal color scheme.

#### Tearing our corrupt output

This is usually and issue with ncurses versions past versions 6.0+20170729. Try changing the TERM variable to rxvt with

    export TERM=rxvt-256color

### Mac OSX

#### vis hangs with no output #1

It is possible there is a naming conflict with an existing BSD tool `vis`. Try running `vis -h`. If the output looks something like

    mac ❯❯❯ vis -h
    vis: illegal option -- h
    usage: vis [-cbflnostw] [-F foldwidth] [file ...]

then there is a naming conflict with cli-visualizer. To fix this issue, put `/usr/local/bin` before `/usr/bin/` in `$PATH`.

#### vis hangs with no output #2

MPD on mac os does not automatically create the output file for the visualizer.

    mkfifo /tmp/mpd.fifo

Running `mkfifo` will create the output file. This file is usually `/tmp/mpd.fifo`, but please make sure it matches any overrides set in your `mpd.conf` and vis config `~/.config/vis/config`.
