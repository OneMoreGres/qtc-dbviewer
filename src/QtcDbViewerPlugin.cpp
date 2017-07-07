#include "QtcDbViewerPlugin.h"
#include "Constants.h"
#include "WMain.h"

#include <QCoreApplication>
#include <QTranslator>

#include <QtPlugin>

#include <coreplugin/icore.h>
#include <coreplugin/imode.h>
#include <utils/icon.h>
#include <coreplugin/actionmanager/actionmanager.h>

using namespace QtcDbViewer::Internal;

const Utils::Icon MODE_DATABASE_CLASSIC (
  QLatin1String (":/icons/database.png"));
const Utils::Icon MODE_DATABASE_FLAT ({
  {QLatin1String (":/icons/database_mask.png"), Utils::Theme::IconsBaseColor}
});
const Utils::Icon MODE_DATABASE_FLAT_ACTIVE ({
  {QLatin1String (":/icons/database_mask.png"), Utils::Theme::IconsModeDesignActiveColor}
});

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

  auto context = Core::Context (Constants::QTCDBVIEWER_CONTEXT);

  auto viewer = new WMain;
  auto buttons = viewer->findChildren<QToolButton *>();
  for (auto button: buttons) {
    if (button->toolTip ().isEmpty ()) {
      continue;
    }
    auto *proxyAction = new QAction (button->toolTip (), button);
    connect (proxyAction, SIGNAL (triggered (bool)), button, SIGNAL (clicked (bool)));

    auto id = Core::Id::fromString (QStringLiteral ("QtcDbViewer.") + button->objectName ());
    auto *cmd = Core::ActionManager::registerAction (proxyAction, id, context);
    cmd->setDefaultKeySequence (button->shortcut ().toString ());
    button->setShortcut (QString ());
    connect (cmd, &Core::Command::keySequenceChanged, [button, proxyAction, cmd]() {
      QString tooltip = proxyAction->text ();
      if (!cmd->keySequence ().isEmpty ()) {
        tooltip += QString (QStringLiteral (" (%1)")).arg (cmd->keySequence ().toString ());
      }
      button->setToolTip (tooltip);
    });
    cmd->keySequenceChanged ();
  }

  Core::IMode *dbViewMode = new Core::IMode;
  dbViewMode->setId (Constants::QTCDBVIEWER_ID);
  dbViewMode->setContext (context);
  dbViewMode->setDisplayName (tr ("Db Viewer"));
  dbViewMode->setIcon (Utils::Icon::modeIcon (MODE_DATABASE_CLASSIC,
                                              MODE_DATABASE_FLAT, MODE_DATABASE_FLAT_ACTIVE));
  dbViewMode->setPriority (10);
  dbViewMode->setWidget (viewer);
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
