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

    void finished(QNetworkReply *reply);


private:
    Ui::GameLauncher *ui;
    QStringList parseVersionString(QString version);
    bool isVersionDownloadedNewer();
    void getNewerGameVersion();
    void updateDownloadUpdateButtonGUI();
    void loadSettings();
    void saveVersionInSetting();
    void isFirstInstall();
    void getVersionOfGameOnWebsite();
    void updateVersionNumberGUI();
    QNetworkAccessManager manager;
    QString versionDownload;
    QString versionSystem;
    QString urlToVersion;
    QString urlToGame;

};
#endif // GAMELAUNCHER_H
