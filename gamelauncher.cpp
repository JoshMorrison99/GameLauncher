#include "gamelauncher.h"
#include "ui_gamelauncher.h"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::finished);

    loadSettings();
    isFirstInstall();
    getVersionOfGameOnWebsite();
    updateVersionNumberGUI();
}

GameLauncher::~GameLauncher()
{
    delete ui;
}

void GameLauncher::getVersionOfGameOnWebsite()
{
    qInfo() << "getVersionOfGameOnWebsite";
    urlToVersion = "https://drive.google.com/uc?export=download&id=1G_lsCH--LZo3U9rWQr_45Z_rYBVq1Vvr";
    manager.get(QNetworkRequest(QUrl(urlToVersion)));
}

void GameLauncher::updateVersionNumberGUI()
{
    QSettings settings("Shelledware", "Prime Piano");
    ui->versionNumber->setText(settings.value("version").toString());
}

void GameLauncher::finished(QNetworkReply *reply)
{
    qInfo() << "finished";
    versionDownload = reply->readAll();
    updateDownloadUpdateButtonGUI();
}

QStringList GameLauncher::parseVersionString(QString version)
{
    qInfo() << "parseVersionString";
    version = version.trimmed();
    QStringList versionNumbers = version.split(".");
    return versionNumbers;
}

bool GameLauncher::isVersionDownloadedNewer()
{
    qInfo() << "isVersionDownloadedNewer";
    qInfo() << versionDownload;
    qInfo() << versionSystem;
    QStringList versionDownloadList = parseVersionString(versionDownload);
    int majorNumberDownloaded = versionDownloadList[0].toInt();
    int minorNumberDownloaded = versionDownloadList[1].toInt();
    int subMijorNumberDownloaded = versionDownloadList[2].toInt();

    QStringList versionSystemList = parseVersionString(versionSystem);
    int majorNumberSystem = versionSystemList[0].toInt();
    int minorNumberSystem = versionSystemList[1].toInt();
    int subMijorNumberSystem = versionSystemList[2].toInt();

    qInfo() << majorNumberDownloaded << "   " << majorNumberSystem;
    qInfo() << minorNumberDownloaded << "   " << minorNumberSystem;
    qInfo() << subMijorNumberDownloaded << "   " << subMijorNumberSystem;

    // Check if version number downloaded from website is different than the verison number in QSettings
    if(majorNumberDownloaded == majorNumberSystem && minorNumberDownloaded == minorNumberSystem && subMijorNumberDownloaded == subMijorNumberSystem)
    {
        return false;
    }
    return true;
}

void GameLauncher::getNewerGameVersion()
{
    // Download the actual unity game
}

void GameLauncher::updateDownloadUpdateButtonGUI()
{
    qInfo() << "updateDownloadUpdateButtonGUI";
    if(isVersionDownloadedNewer())
    {
        ui->play_update_button->setText("Update");
    }
    else
    {
        ui->play_update_button->setText("Play");
    }
}

void GameLauncher::loadSettings()
{
    qInfo() << "loadSettings";
    QSettings settings("Shelledware", "Prime Piano");
    versionSystem = settings.value("version").toString();
}

void GameLauncher::saveVersionInSetting()
{
    qInfo() << "saveVersionInSetting";
    QSettings settings("Shelledware", "Prime Piano");
    settings.setValue("version", versionDownload);
}

void GameLauncher::isFirstInstall()
{
    qInfo() << "isFirstInstall";
    QSettings settings("Shelledware", "Prime Piano");
    if(settings.value("version") == "")
    {
        settings.setValue("version", "0.0.0");
    }



    //settings.setValue("version", "0.0.0"); //Remove for production, use in debugging to reset the version number
}



void GameLauncher::on_play_update_button_clicked()
{
    if(ui->play_update_button->text().toLower() == "update")
    {
        getNewerGameVersion();
        saveVersionInSetting();
    }else{
        // Boot up the unity game
    }
}




