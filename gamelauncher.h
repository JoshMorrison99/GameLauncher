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
#include <update.h>

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


public slots:
    void on_play_update_button_clicked();
    void GameDownloadFinished(QNetworkReply *reply);
    void GameVersionDownloadFinished(QNetworkReply *reply);
    void LauncherVersionDownloadFinished(QNetworkReply *reply);
    void isGameVersionDownloadedNewer();
    void isLauncherVersionDownloadedNewer();
    void ContinueToGame();
    void UpdateLauncher();


signals:
    void GameVersionDownloaded();


private:
    Ui::GameLauncher *ui;

    // Helper Functions
    QStringList ParseVersionString(QString version);

    // First Install Logic
    bool isFirstInstall();
    void FetchGameVersionNumberFromWebsite();
    void FetchLauncherVersionNumberFromWebsite();

    // Game Install Logic
    QString GetVersionOfGameOnWebsite();

    void GetNewerGameVersion();



    // Launcher Install Logic
    QString GetVersionOfLauncherOnWebsite();
    void UpdateLauncherPrompt();

    // GUI Logic
    void UpdateDownloadUpdateButtonGUI();
    void SetGameVersionNumberGUI();
    void SetLauncherVersionNumberGUI();

    // Settings Logic
    void LoadSettings();
    void SaveGameVersionInSetting();

    // Variables
    QNetworkAccessManager manager;
    Update updateWindow;
    QString gameVersionDownload;
    QString gameVersionSystem;
    QString launcherVersionDownload;
    QString launcherVersionSystem;
    QString urlToGameVersion;
    QString urlToLauncherVersion;
    QString urlToGame;
    QString urlToLauncher;
    bool isDownloadedVersionNewer;
    bool isDownloadedVersionLauncherNewer;
};
#endif // GAMELAUNCHER_H
