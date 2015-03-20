#include "QtcDbViewerPlugin.h"
#include "Constants.h"
#include "WMain.h"

#include <QCoreApplication>
#include <QTranslator>

#include <QtPlugin>

#include <coreplugin/icore.h>
#include <coreplugin/imode.h>

using namespace QtcDbViewer::Internal;

QtcDbViewerPlugin::QtcDbViewerPlugin () :
  IPlugin () {
  // Create your members
}

QtcDbViewerPlugin::~QtcDbViewerPlugin () {
  // Unregister objects from the plugin manager's object pool
  // Delete members
}

bool QtcDbViewerPlugin::initialize (const QStringList &arguments, QString *errorString) {
  // Register objects in the plugin manager's object pool
  // Load settings
  // Add actions to menus
  // Connect to other plugins' signals
  // In the initialize function, a plugin can be sure that the plugins it
  // depends on have initialized their members.

  Q_UNUSED (arguments)
  Q_UNUSED (errorString)

  initLanguage ();

  Core::IMode *dbViewMode = new Core::IMode;
  dbViewMode->setId (Constants::QTCDBVIEWER_ID);
  dbViewMode->setContext (Core::Context (Constants::QTCDBVIEWER_CONTEXT));
  dbViewMode->setDisplayName (tr ("Db Viewer"));
  dbViewMode->setIcon (QIcon (QStringLiteral (":/icons/database.png")));
  dbViewMode->setPriority (10);
  dbViewMode->setWidget (new WMain);
  addAutoReleasedObject (dbViewMode);

  return true;
}

void QtcDbViewerPlugin::initLanguage () {
  const QString &language = Core::ICore::userInterfaceLanguage ();
  if (!language.isEmpty ()) {
    QStringList paths;
    paths << Core::ICore::resourcePath () << Core::ICore::userResourcePath ();
    const QString &trFile = QLatin1String ("QtcDbViewer_") + language;
    QTranslator *translator = new QTranslator (this);
    foreach (const QString &path, paths) {
      if (translator->load (trFile, path + QLatin1String ("/translations"))) {
        qApp->installTranslator (translator);
        break;
      }
    }
  }
}

void QtcDbViewerPlugin::extensionsInitialized () {
  // Retrieve objects from the plugin manager's object pool
  // In the extensionsInitialized function, a plugin can be sure that all
  // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag QtcDbViewerPlugin::aboutToShutdown () {
  // Save settings
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)
  return SynchronousShutdown;
}
