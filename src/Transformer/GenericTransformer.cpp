/*
 * GenericTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/GenericTransformer.h"

vis::GenericTransformer::GenericTransformer(const Settings *const settings)
{
    settings->get_fps();
}

vis::GenericTransformer::~GenericTransformer()
{
}
