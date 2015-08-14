/*
 * vis.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include <iostream>
#include <memory>
#include "Source/AudioSource.h"
#include "Source/MpdAudioSource.h"

int main()
{
    std::cout << "hello world" << std::endl;
    std::unique_ptr<AudioSource> audioSource( new MpdAudioSource() );
}
