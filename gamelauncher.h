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
    void isGameVersionDownloadedNewer();
    void GameDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);


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

    // Game Install Logic
    QString GetVersionOfGameOnWebsite();

    void GetNewerGameVersion();

    // GUI Logic
    void UpdateDownloadUpdateButtonGUI();
    void SetGameVersionNumberGUI();

    // Settings Logic
    void LoadSettings();
    void SaveGameVersionInSetting();

    // Variables
    QNetworkAccessManager manager;
    QString gameVersionDownload;
    QString gameVersionSystem;
    QString urlToGameVersion;
    QString urlToGame;
    bool isDownloadedVersionNewer;
    QString DirectoryLocation = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation).at(1); // USE FOR RELEASE
    //QString DirectoryLocation = "C:/Users/joshm/OneDrive/Desktop/User"; // USE FOR DEBUG
    QString URL_to_game_release = "http://primepianist.com/api/";
    //QString URL_to_game_debug = "http://localhost:5000/api/";
};
#endif // GAMELAUNCHER_H
