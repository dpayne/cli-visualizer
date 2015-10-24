/*
 * SpectrumTransformer.cpp
 *
 * Created on: Jul 30, 2015
 *     Author: dpayne
 */

#include "Transformer/SpectrumTransformer.h"

vis::SpectrumTransformer::SpectrumTransformer(const Settings * const settings) : m_settings{ settings }
{
    m_settings->get_mpd_fifo_path();
}

vis::SpectrumTransformer::~SpectrumTransformer()
{
}
