/*
 * Window handling from KWin's Glide effect:
 * SPDX-FileCopyrightText: 2007 Philip Falkner <philip.falkner@gmail.com>
 * SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2010 Alexandre Pereira <pereira.alex@gmail.com>
 * SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "effect/effectwindow.h"
#include "effect/offscreeneffect.h"
#include "effect/timeline.h"
#include "scene/item.h"

#include <QVector3D>

#include <unordered_map>

namespace KWin
{

struct OpenCloseAnimation
{
    EffectWindowDeletedRef deletedRef;
    TimeLine timeLine;
    ItemEffect effect;
};

class OpenCloseEffect : public OffscreenEffect
{
    Q_OBJECT

public:
    OpenCloseEffect();
    ~OpenCloseEffect() override;

    void reconfigure(ReconfigureFlags flags) override;
    void prePaintWindow(RenderView *view, EffectWindow *w, WindowPrePaintData &data) override;
    void postPaintScreen() override;
    bool isActive() const override;
    int requestedEffectChainPosition() const override;
    bool blocksDirectScanout() const override;

    static bool supported();

    // Transforms of the window in units of its own size, at the start and
    // the end.
    struct AnimationParams
    {
        std::chrono::milliseconds duration;
        QEasingCurve curve;
        QVector3D translationFrom, translationTo;
        QVector3D rotationFrom, rotationTo; // degrees around x, y, z
        QVector3D origin;
        qreal opacityFrom, opacityTo;
    };

protected:
    void apply(EffectWindow *window, int mask, WindowPaintData &data, WindowQuadList &quads) override;

private Q_SLOTS:
    void windowAdded(EffectWindow *w);
    void windowClosed(EffectWindow *w);
    void windowDataChanged(EffectWindow *w, int role);

private:
    bool isAnimatedWindow(EffectWindow *w) const;
    void start(EffectWindow *w, const AnimationParams &params);

    AnimationParams m_open;
    AnimationParams m_close;
    std::unordered_map<EffectWindow *, OpenCloseAnimation> m_animations;
};

inline int OpenCloseEffect::requestedEffectChainPosition() const
{
    return 50;
}

} // namespace KWin
