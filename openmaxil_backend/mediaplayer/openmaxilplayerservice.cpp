/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    04.14.2013
 * Project: OpenMAXIL QtMultimedia Plugin
 */

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>

#if defined(HAVE_WIDGETS)
#include <QtWidgets/qwidget.h>
#endif

#include "openmaxilplayerservice.h"
#include "openmaxilplayercontrol.h"
#include "openmaxilmetadataprovider.h"
#include "openmaxilavailabilitycontrol.h"
#include "openmaxilvideorenderercontrol.h"

#if defined(HAVE_WIDGETS)
#include <private/qgstreamervideooverlay_p.h>
#include <private/qgstreamervideowindow_p.h>
#include <private/qgstreamervideowidget_p.h>
#endif

#if defined(Q_WS_MAEMO_6) && defined(__arm__)
#include "qgstreamergltexturerenderer.h"
#endif

#include "openmaxilstreamscontrol.h"
#include <private/qgstreameraudioprobecontrol_p.h>
#include <private/qgstreamervideoprobecontrol_p.h>

#include <private/qmediaplaylistnavigator_p.h>
#include <qmediaplaylist.h>
#include <private/qmediaresourceset_p.h>

QT_BEGIN_NAMESPACE

QGstreamerPlayerService::QGstreamerPlayerService(QObject *parent):
     QMediaService(parent)
     , m_videoOutput(0)
     , m_videoRenderer(0)
#if defined(HAVE_XVIDEO) && defined(HAVE_WIDGETS)
     , m_videoWindow(0)
     , m_videoWidget(0)
#endif
     , m_videoReferenceCount(0)
{
    qDebug("Instantiating QMediaService...");

    m_control             = new OpenMAXILPlayerControl(this);
    m_metaData            = new QGstreamerMetaDataProvider(this);
    m_streamsControl      = new QGstreamerStreamsControl(this);
    m_availabilityControl = new OpenMAXILAvailabilityControl(this);

#if defined(Q_WS_MAEMO_6) && defined(__arm__)
    m_videoRenderer = new QGstreamerGLTextureRenderer(this);
#else
    //m_videoRenderer = new QGstreamerVideoRenderer(this);
    m_videoRenderer = new OpenMAXILVideoRendererControl(this);

    connect(m_control, SIGNAL(textureInvalidated()),
            m_videoRenderer, SLOT(onTextureInvalidated()));
    connect(m_control, SIGNAL(textureReady(const OMX_TextureData*)),
            m_videoRenderer, SLOT(onTextureReady(const OMX_TextureData*)));
#endif

#if defined(HAVE_XVIDEO) && defined(HAVE_WIDGETS)
#ifdef Q_WS_MAEMO_6
    m_videoWindow = new QGstreamerVideoWindow(this, "omapxvsink");
#else
    m_videoWindow = new QGstreamerVideoOverlay(this);
#endif
    m_videoWidget = new QGstreamerVideoWidgetControl(this);
#endif
}

QGstreamerPlayerService::~QGstreamerPlayerService()
{
}

QMediaControl *QGstreamerPlayerService::requestControl(const char *name)
{
    qDebug("Requesting control for %s...", name);

    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name,QMetaDataReaderControl_iid) == 0)
        return 0;

    if (qstrcmp(name,QMediaStreamsControl_iid) == 0)
        return 0;

    if (qstrcmp(name, QMediaAvailabilityControl_iid) == 0)
        return 0;

    if (qstrcmp(name, QMediaVideoProbeControl_iid) == 0) {
        return 0;
        //if (m_session) {
        //    QGstreamerVideoProbeControl *probe = new QGstreamerVideoProbeControl(this);
        //    increaseVideoRef();
        //    m_session->addProbe(probe);
        //    return probe;
        //}
        //return 0;
    }

    if (qstrcmp(name,QMediaAudioProbeControl_iid) == 0) {
        //if (m_session) {
        //    QGstreamerAudioProbeControl *probe = new QGstreamerAudioProbeControl(this);
        //    m_session->addProbe(probe);
        //    return probe;
        //}
        return 0;
    }

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
       return m_videoRenderer;

    /*if (!m_videoOutput) {
        if (qstrcmp(name, QVideoRendererControl_iid) == 0)
            m_videoOutput = m_videoRenderer;
#if defined(HAVE_XVIDEO) && defined(HAVE_WIDGETS)
        else if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
            m_videoOutput = m_videoWidget;
        else  if (qstrcmp(name, QVideoWindowControl_iid) == 0)
            m_videoOutput = m_videoWindow;
#endif

        if (m_videoOutput) {
            increaseVideoRef();
            m_control->setVideoOutput(m_videoOutput);
            return m_videoOutput;
        }
    }*/

    return 0;
}

void QGstreamerPlayerService::releaseControl(QMediaControl *control)
{
#if 0
    if (control == m_videoOutput) {
        m_videoOutput = 0;
        m_control->setVideoOutput(0);
        decreaseVideoRef();
    }

    QGstreamerVideoProbeControl* videoProbe = qobject_cast<QGstreamerVideoProbeControl*>(control);
    if (videoProbe) {
        if (m_session) {
            m_session->removeProbe(videoProbe);
            decreaseVideoRef();
        }
        delete videoProbe;
        return;
    }

    QGstreamerAudioProbeControl* audioProbe = qobject_cast<QGstreamerAudioProbeControl*>(control);
    if (audioProbe) {
        if (m_session)
            m_session->removeProbe(audioProbe);
        delete audioProbe;
        return;
    }
#endif
}

QT_END_NAMESPACE
