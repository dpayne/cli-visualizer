# cli-visualizer

Command line visualizer. Currently only supports mpd.

This project was heavily inspired by [C.A.V.A](https://github.com/karlstav/cava), [ncmpcpp](http://rybczak.net/ncmpcpp/), and [rainbow](https://github.com/sickill/rainbow)

## Installing Pre-requisites

`fftw` and `ncursesw` libraries are required to build. Note that ncurses with wide character support is
needed.

### Ubuntu

  sudo apt-get install libncursesw5 libfftw3-3

### Arch Linux

In arch, the ncursesw is bundled with the ncurses package.

  sudo pacman -S ncurses fftw

### Mac OS X

Mac os x has a version of ncurses builtin, but a newer version is required.

  brew install fftw
  brew tap homebrew/dupes
  brew install ncurses

## Installing Pre-requisites

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
