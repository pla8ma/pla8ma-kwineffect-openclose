/*
 * Window handling from KWin's Glide effect:
 * SPDX-FileCopyrightText: 2007 Philip Falkner <philip.falkner@gmail.com>
 * SPDX-FileCopyrightText: 2009 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2010 Alexandre Pereira <pereira.alex@gmail.com>
 * SPDX-FileCopyrightText: 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "openclose.h"

#include "effect/effecthandler.h"
#include "scene/windowitem.h"

#include <QSet>
#include <qmath.h>

using namespace std::chrono_literals;

namespace KWin
{

static const QSet<QString> s_blacklist{
    QStringLiteral("ksmserver ksmserver"),
    QStringLiteral("ksmserver-logout-greeter ksmserver-logout-greeter"),
    QStringLiteral("ksplashqml ksplashqml"),
};

static qreal cubicDecelerate(qreal u)
{
    return ((u - 2.5) * u + 2.5) * u;
}

static qreal parabolicAccelerate(qreal u)
{
    return u * u;
}

static QEasingCurve curve(QEasingCurve::EasingFunction function)
{
    QEasingCurve easing;
    easing.setCustomType(function);
    return easing;
}

// Rotation by yaw/pitch/roll (z, then x, then y) as a matrix for row
// vectors: p' = p * m.
static void rotationMatrix(const QVector3D &degrees, qreal m[3][3])
{
    const qreal hx = qDegreesToRadians(degrees.x()) / 2;
    const qreal hy = qDegreesToRadians(degrees.y()) / 2;
    const qreal hz = qDegreesToRadians(degrees.z()) / 2;
    const qreal sx = std::sin(hx), cx = std::cos(hx);
    const qreal sy = std::sin(hy), cy = std::cos(hy);
    const qreal sz = std::sin(hz), cz = std::cos(hz);

    const qreal x = cy * sx * cz + sy * cx * sz;
    const qreal y = sy * cx * cz - cy * sx * sz;
    const qreal z = cy * cx * sz - sy * sx * cz;
    const qreal w = cy * cx * cz + sy * sx * sz;

    m[0][0] = 1 - 2 * (y * y + z * z);
    m[0][1] = 2 * (x * y + z * w);
    m[0][2] = 2 * (x * z - y * w);
    m[1][0] = 2 * (x * y - z * w);
    m[1][1] = 1 - 2 * (x * x + z * z);
    m[1][2] = 2 * (y * z + x * w);
    m[2][0] = 2 * (x * z + y * w);
    m[2][1] = 2 * (y * z - x * w);
    m[2][2] = 1 - 2 * (x * x + y * y);
}

OpenCloseEffect::OpenCloseEffect()
{
    reconfigure(ReconfigureAll);

    connect(effects, &EffectsHandler::windowAdded, this, &OpenCloseEffect::windowAdded);
    connect(effects, &EffectsHandler::windowClosed, this, &OpenCloseEffect::windowClosed);
    connect(effects, &EffectsHandler::windowDataChanged, this, &OpenCloseEffect::windowDataChanged);
}

OpenCloseEffect::~OpenCloseEffect() = default;

void OpenCloseEffect::reconfigure(ReconfigureFlags)
{
    m_open = AnimationParams{
        .duration = animationTime(250ms),
        .curve = curve(cubicDecelerate),
        .translationFrom = QVector3D(0, -0.017, -0.1),
        .translationTo = QVector3D(0, 0, 0),
        .rotationFrom = QVector3D(-5, 0, 0),
        .rotationTo = QVector3D(0, 0, 0),
        .origin = QVector3D(0, 0.5, 0),
        .opacityFrom = 0,
        .opacityTo = 1,
    };
    m_close = AnimationParams{
        .duration = animationTime(166ms),
        .curve = curve(parabolicAccelerate),
        .translationFrom = QVector3D(0, 0, 0),
        .translationTo = QVector3D(0, 0, 0),
        .rotationFrom = QVector3D(0, 0, 0),
        .rotationTo = QVector3D(5, 2, 0),
        .origin = QVector3D(0, 1, 0),
        .opacityFrom = 1,
        .opacityTo = 0,
    };
}

void OpenCloseEffect::prePaintWindow(RenderView *view, EffectWindow *w, WindowPrePaintData &data)
{
    auto animationIt = m_animations.find(w);
    if (animationIt != m_animations.end()) {
        animationIt->second.timeLine.advance(view);
        data.setTransformed();
    }

    effects->prePaintWindow(view, w, data);
}

void OpenCloseEffect::apply(EffectWindow *window, int, WindowPaintData &data, WindowQuadList &quads)
{
    auto animationIt = m_animations.find(window);
    if (animationIt == m_animations.end()) {
        return;
    }

    const AnimationParams &params = window->isDeleted() ? m_close : m_open;
    const qreal t = animationIt->second.timeLine.value();
    const QVector3D translation = params.translationFrom + (params.translationTo - params.translationFrom) * t;
    const QVector3D rotation = params.rotationFrom + (params.rotationTo - params.rotationFrom) * t;
    const QVector3D &origin = params.origin;

    qreal m[3][3];
    rotationMatrix(rotation, m);

    // Quads are relative to the frame's top left corner; the shadow lies
    // outside the unit square and goes along.
    const qreal width = window->width();
    const qreal height = window->height();

    quads = quads.makeRegularGrid(20, 20);
    for (WindowQuad &quad : quads) {
        for (int i = 0; i < 4; ++i) {
            const qreal px = quad[i].x() / width - origin.x();
            const qreal py = quad[i].y() / height - origin.y();
            const qreal pz = -origin.z();

            const qreal x = px * m[0][0] + py * m[1][0] + pz * m[2][0] + origin.x() + translation.x();
            const qreal y = px * m[0][1] + py * m[1][1] + pz * m[2][1] + origin.y() + translation.y();
            const qreal z = px * m[0][2] + py * m[1][2] + pz * m[2][2] + origin.z() + translation.z();

            const qreal perspective = 1 / (1 - z);
            quad[i].setX((0.5 + (x - 0.5) * perspective) * width);
            quad[i].setY((0.5 + (y - 0.5) * perspective) * height);
        }
    }

    data.multiplyOpacity(params.opacityFrom + (params.opacityTo - params.opacityFrom) * t);
}

void OpenCloseEffect::postPaintScreen()
{
    for (auto animationIt = m_animations.begin(); animationIt != m_animations.end();) {
        EffectWindow *w = animationIt->first;
        w->addRepaintFull();

        if (animationIt->second.timeLine.done()) {
            unredirect(animationIt->first);
            animationIt = m_animations.erase(animationIt);
        } else {
            ++animationIt;
        }
    }

    effects->postPaintScreen();
}

bool OpenCloseEffect::isActive() const
{
    return !m_animations.empty();
}

bool OpenCloseEffect::blocksDirectScanout() const
{
    return false;
}

bool OpenCloseEffect::supported()
{
    return effects->isOpenGLCompositing()
        && effects->animationsSupported();
}

void OpenCloseEffect::start(EffectWindow *w, const AnimationParams &params)
{
    OpenCloseAnimation &animation = m_animations[w];
    animation.timeLine.reset();
    animation.timeLine.setDirection(TimeLine::Forward);
    animation.timeLine.setDuration(params.duration);
    animation.timeLine.setEasingCurve(params.curve);

    redirect(w);
    effects->addRepaintFull();
}

void OpenCloseEffect::windowAdded(EffectWindow *w)
{
    if (effects->activeFullScreenEffect() || !isAnimatedWindow(w) || !w->isVisible()) {
        return;
    }

    const void *addGrab = w->data(WindowAddedGrabRole).value<void *>();
    if (addGrab) {
        return;
    }

    m_animations[w].effect = ItemEffect(w->windowItem());
    start(w, m_open);
}

void OpenCloseEffect::windowClosed(EffectWindow *w)
{
    const void *closeGrab = w->data(WindowClosedGrabRole).value<void *>();
    if (closeGrab) {
        return;
    }
    if (effects->activeFullScreenEffect() || !isAnimatedWindow(w) || !w->isVisible()
        || w->isMinimized() || w->skipsCloseAnimation()) {
        const auto it = m_animations.find(w);
        if (it != m_animations.end()) {
            unredirect(w);
            m_animations.erase(it);
        }
        return;
    }

    m_animations[w].deletedRef = EffectWindowDeletedRef(w);
    start(w, m_close);
}

void OpenCloseEffect::windowDataChanged(EffectWindow *w, int role)
{
    if (role != WindowAddedGrabRole && role != WindowClosedGrabRole) {
        return;
    }

    auto animationIt = m_animations.find(w);
    if (animationIt != m_animations.end()) {
        unredirect(animationIt->first);
        m_animations.erase(animationIt);
    }
}

bool OpenCloseEffect::isAnimatedWindow(EffectWindow *w) const
{
    // plasmashell's windows share one window class; only its decorated
    // ones (settings dialogs) have a title bar.
    if (w->windowClass() == QLatin1StringView("plasmashell plasmashell")
        || w->windowClass() == QLatin1StringView("plasmashell org.kde.plasmashell")) {
        return w->hasDecoration();
    }

    if (w->windowClass() == QLatin1StringView("spectacle org.kde.spectacle")
        && w->tag() == QLatin1StringView("region-editor")) {
        return false;
    }

    if (s_blacklist.contains(w->windowClass())) {
        return false;
    }

    if (w->hasDecoration()) {
        return true;
    }

    if (w->isPopupWindow() || w->isLockScreen() || w->isOutline()) {
        return false;
    }

    if (w->isX11Client() && !w->isManaged()) {
        return false;
    }

    // Windows that draw their own title bar.
    return w->isNormalWindow() || w->isDialog();
}

} // namespace KWin

#include "moc_openclose.cpp"
