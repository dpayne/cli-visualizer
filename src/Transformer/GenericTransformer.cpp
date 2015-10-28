/*
 * GenericTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/GenericTransformer.h"
#include <sys/ioctl.h>
#include <unistd.h>

vis::GenericTransformer::GenericTransformer()
{
}

vis::GenericTransformer::~GenericTransformer()
{
}

int32_t vis::GenericTransformer::get_window_width() const
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_col;
}

int32_t vis::GenericTransformer::get_window_height() const
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_row;
}
