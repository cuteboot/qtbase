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
#include <QLoggingCategory>

#include "qeglfssurfaceflingerintegration.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcSf, "qt.qpa.eglfs.surfaceflinger")

QEglFSSurfaceFlingerIntegration::QEglFSSurfaceFlingerIntegration()
{
    qCDebug(qLcSf) << "Acquiring token";
    sp<IBinder> dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    qCDebug(qLcSf) << "Token acquired";
    DisplayInfo dinfo;
    mSession = new SurfaceComposerClient();
    qCDebug(qLcSf) << "Getting display info";
    SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    mSize = QSize(dinfo.w, dinfo.h);
    qCDebug(qLcSf) << "Display size: " << mSize;
}

EGLNativeWindowType QEglFSSurfaceFlingerIntegration::createNativeWindow(QPlatformWindow *window, const QSize &size, const QSurfaceFormat &format)
{
    Q_UNUSED(window)
    Q_UNUSED(size);

    qCDebug(qLcSf) << "Creating native window";
    int status=0;
    mControl = mSession->createSurface(android::String8("eglfs"),
            mSize.width(), mSize.height(), PIXEL_FORMAT_RGB_888);
    SurfaceComposerClient::openGlobalTransaction();
    mControl->setLayer(0x40000000);
//    mControl->setAlpha(1);
    SurfaceComposerClient::closeGlobalTransaction();
    mAndroidSurface = mControl->getSurface();

    EGLNativeWindowType eglWindow = mAndroidSurface.get();
    qCDebug(qLcSf) << "Created native window";
    return eglWindow;
}

bool QEglFSSurfaceFlingerIntegration::filterConfig(EGLDisplay display, EGLConfig config) const
{
    return true;
}

QSize QEglFSSurfaceFlingerIntegration::screenSize() const
{
    return mSize;
}

QT_END_NAMESPACE
