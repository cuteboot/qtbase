/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include <QDebug>

#include "qeglfssurfaceflingerintegration.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

QT_BEGIN_NAMESPACE

QEglFSSurfaceFlingerIntegration::QEglFSSurfaceFlingerIntegration()
    : mFramebufferVisualId(EGL_DONT_CARE)
{
    mUseFramebuffer = qgetenv("QT_QPA_EGLFS_NO_SURFACEFLINGER").toInt();
    sp<IBinder> dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    DisplayInfo dinfo;
    mSession = new SurfaceComposerClient();
    SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    mSize = QSize(dinfo.w, dinfo.h);
}

void QEglFSSurfaceFlingerIntegration::ensureFramebufferNativeWindowCreated()
{
    if (mFramebufferNativeWindow.get())
        return;
    mFramebufferNativeWindow = new FramebufferNativeWindow();
    framebuffer_device_t const *fbDev = mFramebufferNativeWindow->getDevice();
    if (!fbDev)
        qFatal("Failed to get valid FramebufferNativeWindow, no way to create EGL surfaces");

    ANativeWindow *window = mFramebufferNativeWindow.get();

    window->query(window, NATIVE_WINDOW_FORMAT, &mFramebufferVisualId);
}

EGLNativeWindowType QEglFSSurfaceFlingerIntegration::createNativeWindow(QPlatformWindow *window, const QSize &size, const QSurfaceFormat &format)
{
    Q_UNUSED(window)
    return mUseFramebuffer ? createNativeWindowFramebuffer(size, format) : createNativeWindowSurfaceFlinger(size, format);
}

EGLNativeWindowType QEglFSSurfaceFlingerIntegration::createNativeWindowFramebuffer(const QSize &size, const QSurfaceFormat &)
{
    Q_UNUSED(size);
    ensureFramebufferNativeWindowCreated();
    return mFramebufferNativeWindow.get();
}

EGLNativeWindowType QEglFSSurfaceFlingerIntegration::createNativeWindowSurfaceFlinger(const QSize &size, const QSurfaceFormat &)
{
    Q_UNUSED(size);

    int status=0;
    mControl = mSession->createSurface(android::String8("eglfs"),
            mSize.width(), mSize.height(), PIXEL_FORMAT_RGB_888);
    SurfaceComposerClient::openGlobalTransaction();
    mControl->setLayer(0x40000000);
//    mControl->setAlpha(1);
    SurfaceComposerClient::closeGlobalTransaction();
    mAndroidSurface = mControl->getSurface();

    EGLNativeWindowType eglWindow = mAndroidSurface.get();
    return eglWindow;
}

bool QEglFSSurfaceFlingerIntegration::filterConfig(EGLDisplay display, EGLConfig config) const
{
    if (!mUseFramebuffer)
        return true;

    const_cast<QEglFSSurfaceFlingerIntegration *>(this)->ensureFramebufferNativeWindowCreated();

    if (mFramebufferVisualId == EGL_DONT_CARE)
        return true;

    EGLint nativeVisualId = 0;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &nativeVisualId);

    return nativeVisualId == mFramebufferVisualId;
}

QSize QEglFSSurfaceFlingerIntegration::screenSize() const
{
    return mSize;
}


QT_END_NAMESPACE
