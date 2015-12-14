/*
 * GenericTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/GenericTransformer.h"
#include "Utils/NcursesUtils.h"
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
    return vis::NcursesUtils::get_window_width();
}

int32_t vis::GenericTransformer::get_window_height() const
{
    return vis::NcursesUtils::get_window_height();
}
