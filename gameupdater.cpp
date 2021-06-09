#include "gameupdater.h"

GameUpdater::GameUpdater()
{
    qDebug() << DirectoryLocation;
}

void GameUpdater::GameDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameDownloadFinished";

    QByteArray newGameDownloadRaw = reply->readAll();
    QFile newGameDownloadFile("UnityGame.zip");
    newGameDownloadFile.open(QIODevice::WriteOnly);
    newGameDownloadFile.write(newGameDownloadRaw);
    newGameDownloadFile.rename(DirectoryLocation + "/tempGame.zip");

    // rename UnityGame.exe to oldGame.exe
    QFile oldGameFile(DirectoryLocation + "/PrimePianistBuilds");
    oldGameFile.open(QIODevice::WriteOnly);
    oldGameFile.rename(DirectoryLocation + "/oldGame");

    // rename tempGame.exe to UnityGame.exe
    newGameDownloadFile.rename(DirectoryLocation + "/UnityGame.zip");

    // delete oldGame.exe version
    oldGameFile.remove();

    // delete oldGame game folder
    QDir oldGameFolder = QDir(DirectoryLocation + "/oldGame");
    oldGameFolder.removeRecursively();

    // Unzip
    QString ZipFile = DirectoryLocation + "/UnityGame.zip";
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

void GameUpdater::SaveGameVersionInSetting()
{
    qInfo() << "saveVersionInSetting";
    QSettings settings(COMPANY, APPLICATION);
    settings.setValue("version_game", gameVersionDownload);
}

void GameUpdater::GameVersionDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameVersionDownloadFinished";
    gameVersionDownload = reply->readAll();
    QSettings settings(COMPANY, APPLICATION);


    disconnect(&manager, &QNetworkAccessManager::finished, this, &GameUpdater::GameVersionDownloadFinished);

    emit GameVersionDownloaded();
}

void GameUpdater::ContinueToGame()
{
    qInfo() << "User doesn't want to update the launcher or launcher doesn't need to be updated";
    isDownloadedVersionLauncherNewer = false;

    // set the gui
    SetLauncherVersionNumberGUI();

    // Update the game if the launcher doesn't need to be updated or user doesn't want to update it
    FetchGameVersionNumberFromWebsite();
    connect(this, &GameUpdater::GameVersionDownloaded, this, &GameUpdater::isGameVersionDownloadedNewer);
}

void GameUpdater::isGameVersionDownloadedNewer()
{
    qInfo() << "isVersionDownloadedNewer";
    //qInfo() << gameVersionDownload;
    //qInfo() << gameVersionSystem;
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

void GameUpdater::GameDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qInfo() << "GameDownloadProgress";
    //qInfo() << "bytesReceived " + QString::number(bytesReceived);
    //qInfo() << "bytesTotal " + QString::number(bytesTotal);
    //ui->progressBar->setValue(bytesReceived/bytesTotal);
}

void GameUpdater::FetchGameVersionNumberFromWebsite()
{
    qInfo() << "FetchGameVersionNumberFromWebsite";
    qInfo() << &manager;
    connect(&manager, &QNetworkAccessManager::finished, this, &GameUpdater::GameVersionDownloadFinished);
    urlToGameVersion = URL_to_game_release + "version_game";
    qInfo() << urlToGameVersion;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(urlToGameVersion)));
    qInfo() << &manager;
    qInfo() << reply->error();
}

void GameUpdater::GetNewerGameVersion()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &GameUpdater::GameDownloadFinished);

    // Download the actual unity game
    urlToGame = URL_to_game_release + "download_game";
    QNetworkRequest request = QNetworkRequest(QUrl(urlToGame));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &GameUpdater::GameDownloadProgress);
}

