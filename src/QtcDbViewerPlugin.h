#ifndef QTCDBVIEWER_H
#define QTCDBVIEWER_H

#include "PluginGlobal.h"

#include <extensionsystem/iplugin.h>

namespace QtcDbViewer {
  namespace Internal {

    class QtcDbViewerPlugin : public ExtensionSystem::IPlugin {
      Q_OBJECT
      Q_PLUGIN_METADATA (IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcDbViewer.json")

      public:
        QtcDbViewerPlugin ();
        ~QtcDbViewerPlugin ();

        bool initialize (const QStringList &arguments, QString *errorString);
        void extensionsInitialized ();
        ShutdownFlag aboutToShutdown ();

      private:
        void initLanguage ();

    };

  } // namespace Internal
} // namespace QtcDbViewer

#endif // QTCDBVIEWER_H

