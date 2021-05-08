#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QSettings>

#define COMPANY "Shelledware"
#define APPLICATION "Prime Pianist"

QT_BEGIN_NAMESPACE
namespace Ui { class GameLauncher; }
QT_END_NAMESPACE

class GameLauncher : public QMainWindow
{
    Q_OBJECT

public:
    GameLauncher(QWidget *parent = nullptr);
    ~GameLauncher();


private slots:
    void on_play_update_button_clicked();
    void GameDownloadFinished(QNetworkReply *reply);
    void GameVersionDownloadFinished(QNetworkReply *reply);





private:
    Ui::GameLauncher *ui;

    // Helper Functions
    QStringList ParseVersionString(QString version);

    // First Install Logic
    bool isFirstInstall();
    void SetupFirstInstall();

    // Game Install Logic
    void GetVersionOfGameOnWebsite();
    void UpdateVersionNumberGUI();
    void GetNewerGameVersion();
    bool isGameVersionDownloadedNewer();


    // Launcher Install Logic

    // GUI Logic
    void UpdateDownloadUpdateButtonGUI();

    // Settings Logic
    void LoadSettings();
    void SaveGameVersionInSetting();

    // Variables
    QNetworkAccessManager manager;
    QString gameVersionDownload;
    QString gameVersionSystem;
    QString launcherVersionDownload;
    QString launcherVersionSystem;
    QString urlToGameVersion;
    QString urlToLauncherVersion;
    QString urlToGame;
    QString urlToLauncher;
};
#endif // GAMELAUNCHER_H
