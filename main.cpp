/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "openclose.h"

namespace KWin
{

KWIN_EFFECT_FACTORY_SUPPORTED(OpenCloseEffect,
                              "metadata.json",
                              return OpenCloseEffect::supported();)

} // namespace KWin

#include "main.moc"
