#include "gamelauncher.h"
#include "ui_gamelauncher.h"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(1280, 680));

    LoadSettings();

    // set ui
    SetGameVersionNumberGUI();

    // Update the game version to check for updates
    FetchGameVersionNumberFromWebsite();

}

GameLauncher::~GameLauncher()
{
    delete ui;
}

// Set the game and launcher versions to the same as the version on the website
void GameLauncher::FetchGameVersionNumberFromWebsite()
{
    qInfo() << "FetchGameVersionNumberFromWebsite";
    qInfo() << &manager;
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);
    urlToGameVersion = URL_to_game_release + "game_version";
    qInfo() << urlToGameVersion;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(urlToGameVersion)));
    qInfo() << &manager;
    qInfo() << reply->error();
}

void GameLauncher::GameVersionDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameVersionDownloadFinished";
    gameVersionDownload = reply->readAll();
    qInfo() << gameVersionDownload;
    QSettings settings(COMPANY, APPLICATION);

    disconnect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);

    isGameVersionDownloadedNewer();
}

void GameLauncher::isGameVersionDownloadedNewer()
{
    qInfo() << "isVersionDownloadedNewer";
    qInfo() << gameVersionDownload;
    qInfo() << gameVersionSystem;
    QStringList versionDownloadList = ParseVersionString(gameVersionDownload);
    int majorNumberDownloaded = versionDownloadList[0].toInt();
    int minorNumberDownloaded = versionDownloadList[1].toInt();
    int subMijorNumberDownloaded = versionDownloadList[2].toInt();

    QStringList versionSystemList = ParseVersionString(gameVersionSystem);
    int majorNumberSystem = versionSystemList[0].toInt();
    int minorNumberSystem = versionSystemList[1].toInt();
    int subMijorNumberSystem = versionSystemList[2].toInt();

    qInfo() << majorNumberDownloaded << "   " << majorNumberSystem;
    qInfo() << minorNumberDownloaded << "   " << minorNumberSystem;
    qInfo() << subMijorNumberDownloaded << "   " << subMijorNumberSystem;

    // Check if version number downloaded from website is different than the verison number in QSettings
    if(majorNumberDownloaded == majorNumberSystem && minorNumberDownloaded == minorNumberSystem && subMijorNumberDownloaded == subMijorNumberSystem)
    {
        isDownloadedVersionNewer = false;
        UpdateDownloadUpdateButtonGUI();
        return;
    }
    isDownloadedVersionNewer = true;
    UpdateDownloadUpdateButtonGUI();
    return;
}

void GameLauncher::UpdateDownloadUpdateButtonGUI()
{
    qInfo() << "updateDownloadUpdateButtonGUI";
    if(isDownloadedVersionNewer)
    {
        ui->play_update_button->setText("Update");
        ui->progressBar->setValue(0);
    }
    else
    {
        ui->play_update_button->setText("Play");
        ui->progressBar->hide();
        ui->play_update_button->resize(ui->play_update_button->width() * 2, ui->play_update_button->height());
        ui->play_update_button->move((this->width() / 2 - ui->play_update_button->width() / 2) , ui->play_update_button->y());

    }
}

void GameLauncher::on_play_update_button_clicked()
{
    if(ui->play_update_button->text().toLower() == "update")
    {
        GetNewerGameVersion();
    }else{
        // Boot up the unity game
        QDesktopServices::openUrl(QUrl::fromLocalFile(DirectoryLocation + "/PrimePianistBuilds/Prime Pianist.exe"));
    }
}

void GameLauncher::GetNewerGameVersion()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameDownloadFinished);

    // Download the actual unity game
    urlToGame = URL_to_game_release + "game_download";
    QNetworkRequest request = QNetworkRequest(QUrl(urlToGame));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &GameLauncher::GameDownloadProgress);
}

void GameLauncher::GameDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameDownloadFinished";

    QByteArray newGameDownloadRaw = reply->readAll();
    QFile newGameDownloadFile("download_game.zip");
    newGameDownloadFile.open(QIODevice::WriteOnly);
    newGameDownloadFile.write(newGameDownloadRaw);
    newGameDownloadFile.rename(DirectoryLocation + "/tempGame.zip");

    // rename tempGame.exe to UnityGame.exe
    newGameDownloadFile.rename(DirectoryLocation + "/download_game.zip");

    // delete oldGame game folder
    QDir oldGameFolder = QDir(DirectoryLocation + "/PrimePianistBuilds");
    oldGameFolder.removeRecursively();

    // Unzip
    QString ZipFile = DirectoryLocation + "/download_game.zip";
    QString Directory = DirectoryLocation;
    JlCompress::extractDir(ZipFile, Directory);

    // Remove Zip file
    newGameDownloadFile.remove();

    // update settings for currently installed game version
    SaveGameVersionInSetting();

    // update version gui
    SetGameVersionNumberGUI();
    isDownloadedVersionNewer = false;
    UpdateDownloadUpdateButtonGUI();
}


QStringList GameLauncher::ParseVersionString(QString version)
{
    qInfo() << "parseVersionString";
    version = version.trimmed();
    QStringList versionNumbers = version.split(".");
    return versionNumbers;
}



void GameLauncher::GameDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qInfo() << "GameDownloadProgress";
    qInfo() << "bytesReceived " + QString::number(bytesReceived);
    qInfo() << "bytesTotal " + QString::number(bytesTotal);
    ui->progressBar->setValue(bytesReceived/bytesTotal);
}




void GameLauncher::LoadSettings()
{
    qInfo() << "loadSettings";
    QSettings settings(COMPANY, APPLICATION);

    // GAME
    gameVersionSystem = settings.value("version_game").toString();
}

void GameLauncher::SaveGameVersionInSetting()
{
    qInfo() << "saveVersionInSetting";
    QSettings settings(COMPANY, APPLICATION);
    settings.setValue("version_game", gameVersionDownload);
}




bool GameLauncher::isFirstInstall()
{
    qInfo() << "isFirstInstall";
    QSettings settings(COMPANY, APPLICATION);
    if(settings.value("version_game").toString() == "")
    {
        qInfo() << "YES";
        return true;
    }
    qInfo() << "NO";
    return false;
}

void GameLauncher::SetGameVersionNumberGUI()
{
    QSettings settings(COMPANY, APPLICATION);
    ui->versionGameLabel->setText(settings.value("version_game").toString());
}


void GameLauncher::on_youtube_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.youtube.com/channel/UCDj-K11fBHMZB7w31T3X08A"));
}

void GameLauncher::on_discord_clicked()
{
    QDesktopServices::openUrl(QUrl("https://discord.gg/EuesAp9c"));
}

void GameLauncher::on_instagram_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.instagram.com/primepianist/"));
}

void GameLauncher::on_twitter_clicked()
{
    QDesktopServices::openUrl(QUrl("https://twitter.com/PianistPrime"));
}

void GameLauncher::on_websiteButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://primepianist.com"));
}

void GameLauncher::on_supportBtn_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.patreon.com/primepianist"));
}

void GameLauncher::on_supportBtn_2_clicked()
{
    QDesktopServices::openUrl(QUrl("https://primepianist.com/logs"));
}
