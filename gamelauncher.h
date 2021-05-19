#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QUrl>
#include <QSettings>
#include <QApplication>
#include <QProcess>
#include <QDesktopServices>
#include <QStandardPaths>
#include <JlCompress.h>


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
    void LauncherDownloadFinished(QNetworkReply *reply);
    void isGameVersionDownloadedNewer();
    void isLauncherVersionDownloadedNewer();
    void ContinueToGame();
    void UpdateLauncher();
    void GameDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void LauncherDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);


signals:
    void GameVersionDownloaded();


private slots:
    void on_youtube_clicked();

    void on_discord_clicked();

    void on_instagram_clicked();

    void on_twitter_clicked();

    void on_websiteButton_clicked();

    void on_supportBtn_clicked();

    void on_supportBtn_2_clicked();

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
    void DeleteOldLauncherIfExists();

    // GUI Logic
    void UpdateDownloadUpdateButtonGUI();
    void SetGameVersionNumberGUI();
    void SetLauncherVersionNumberGUI();

    // Settings Logic
    void LoadSettings();
    void SaveGameVersionInSetting();
    void SaveLauncherVersionInSetting();

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
    QString DirectoryLocation = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation).at(1); // USE FOR RELEASE
    //QString DirectoryLocation = "C:/Users/joshm/OneDrive/Desktop/User"; // USE FOR DEBUG
    QString URL_to_game_release = "http://primepianist.com/api/";
    //QString URL_to_game_debug = "http://localhost:5000/api/";
    QFile logs;
};
#endif // GAMELAUNCHER_H
