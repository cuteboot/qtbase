/****************************************************************************
**
** Copyright (C) 2015 Robin Burchell <robin+qt@viroteck.net>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QEGLFSSURFACEFLINGERINTEGRATION_H
#define QEGLFSSURFACEFLINGERINTEGRATION_H

#include "qeglfsdeviceintegration.h"

#include <ui/DisplayInfo.h>
#include <ui/FramebufferNativeWindow.h>
//#if Q_ANDROID_VERSION_MAJOR > 4 || (Q_ANDROID_VERSION_MAJOR == 4 && Q_ANDROID_VERSION_MINOR >= 1)
#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <gui/ISurfaceComposer.h>
//#else
//#include <surfaceflinger/SurfaceComposerClient.h>
//#endif

using namespace android;


QT_BEGIN_NAMESPACE

class QEglFSSurfaceFlingerIntegration : public QEGLDeviceIntegration
{
public:
    QEglFSSurfaceFlingerIntegration();
    virtual EGLNativeWindowType createNativeWindow(QPlatformWindow *window, const QSize &size, const QSurfaceFormat &format);
    virtual bool filterConfig(EGLDisplay display, EGLConfig config) const;
    virtual QByteArray fbDeviceName() const { return "/dev/graphics/fb0"; }
    virtual void platformInit() {}
    virtual void platformDestroy() {}
    QSize screenSize() const;

private:
    EGLNativeWindowType createNativeWindowSurfaceFlinger(const QSize &size, const QSurfaceFormat &format);
    EGLNativeWindowType createNativeWindowFramebuffer(const QSize &size, const QSurfaceFormat &format);

    void ensureFramebufferNativeWindowCreated();

    // androidy things
    sp<android::SurfaceComposerClient> mSession;
    sp<android::SurfaceControl> mControl;
    sp<android::Surface> mAndroidSurface;

    sp<android::FramebufferNativeWindow> mFramebufferNativeWindow;
    EGLint mFramebufferVisualId;

    bool mUseFramebuffer;

    QSize mSize;
};

QT_END_NAMESPACE

#endif // QEGLFSSURFACEFLINGERINTEGRATION_H
