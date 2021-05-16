#include "gamelauncher.h"
#include "ui_gamelauncher.h"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(1280, 680));

    LoadSettings();

    // Check if old Launcher exists
    DeleteOldLauncherIfExists();

    // Update the launcher first
    FetchLauncherVersionNumberFromWebsite();



}

GameLauncher::~GameLauncher()
{
    delete ui;
}

void GameLauncher::DeleteOldLauncherIfExists(){
    QDir oldLauncher = QDir("C:/Users/joshm/OneDrive/Desktop/User/oldLauncher");
    if(oldLauncher.exists())
    {
        qInfo() << "Old Launcher Exists... Deleting";
        oldLauncher.removeRecursively();
    }
}

void GameLauncher::GameDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameDownloadFinished";
    //qInfo() << reply->readAll();
    // Download the new version UnityGame.exe to tempGame.exe
    QByteArray newGameDownloadRaw = reply->readAll();
    QFile newGameDownloadFile = QFile("UnityGame.zip");
    newGameDownloadFile.open(QIODevice::WriteOnly);
    newGameDownloadFile.write(newGameDownloadRaw);
    newGameDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/tempGame.zip");

    // rename UnityGame.exe to oldGame.exe
    QFile oldGameFile = QFile("C:/Users/joshm/OneDrive/Desktop/User/PrimePianistBuilds");
    oldGameFile.open(QIODevice::WriteOnly);
    oldGameFile.rename("C:/Users/joshm/OneDrive/Desktop/User/oldGame");

    // rename tempGame.exe to UnityGame.exe
    newGameDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/UnityGame.zip");

    // delete oldGame.exe version
    oldGameFile.remove();

    // delete oldGame game folder
    QDir oldGameFolder = QDir("C:/Users/joshm/OneDrive/Desktop/User/oldGame");
    oldGameFolder.removeRecursively();

    // Unzip
    QProcess p;
    QStringList params;
    QString pythonPath = "C:/Users/joshm/AppData/Local/Microsoft/WindowsApps/PythonSoftwareFoundation.Python.3.9_qbz5n2kfra8p0/python3.9.exe";
    QString pythonScript = "C:/Users/joshm/OneDrive/Desktop/unzipper.py";
    QString path_to_launcher_zip = "C:/Users/joshm/OneDrive/Desktop/User/UnityGame.zip";
    QString path_to_destination = "C:/Users/joshm/OneDrive/Desktop/User";
    params << pythonScript << path_to_launcher_zip << path_to_destination;
    p.start(pythonPath, params);
    p.waitForFinished(-1);
    p.close();

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

void GameLauncher::isLauncherVersionDownloadedNewer()
{
    qInfo() << "isLauncherVersionDownloadedNewer";
    qInfo() << launcherVersionDownload;
    qInfo() << launcherVersionSystem;
    QStringList versionDownloadList = ParseVersionString(launcherVersionDownload);
    int majorNumberDownloaded = versionDownloadList[0].toInt();
    int minorNumberDownloaded = versionDownloadList[1].toInt();
    int subMijorNumberDownloaded = versionDownloadList[2].toInt();

    QStringList versionSystemList = ParseVersionString(launcherVersionSystem);
    int majorNumberSystem = versionSystemList[0].toInt();
    int minorNumberSystem = versionSystemList[1].toInt();
    int subMijorNumberSystem = versionSystemList[2].toInt();

    qInfo() << majorNumberDownloaded << "   " << majorNumberSystem;
    qInfo() << minorNumberDownloaded << "   " << minorNumberSystem;
    qInfo() << subMijorNumberDownloaded << "   " << subMijorNumberSystem;

    // Check if version number downloaded from website is different than the verison number in QSettings
    if(majorNumberDownloaded == majorNumberSystem && minorNumberDownloaded == minorNumberSystem && subMijorNumberDownloaded == subMijorNumberSystem)
    {
        qInfo() << "Launcher doesn't need to be updated";
        ContinueToGame();
        return;
    }
    qInfo() << "Launcher needs to be updated";
    connect(&updateWindow, SIGNAL(UserCancelUpdate()), this, SLOT(ContinueToGame()));
    connect(&updateWindow, SIGNAL(OK()), this, SLOT(UpdateLauncher()));
    isDownloadedVersionLauncherNewer = true;
    UpdateLauncherPrompt();
    return;
}

void GameLauncher::ContinueToGame()
{
    qInfo() << "User doesn't want to update the launcher or launcher doesn't need to be updated";
    isDownloadedVersionLauncherNewer = false;

    // set the gui
    SetLauncherVersionNumberGUI();

    // Update the game if the launcher doesn't need to be updated or user doesn't want to update it
    FetchGameVersionNumberFromWebsite();
    connect(this, &GameLauncher::GameVersionDownloaded, this, &GameLauncher::isGameVersionDownloadedNewer);
}

void GameLauncher::UpdateLauncher()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::LauncherDownloadFinished);
    // download the updated launcher
    //urlToGame = "C:/Users/joshm/OneDrive/Desktop/Server/Launcher.exe";
    urlToGame = "http://localhost:5000/api/download_launcher";
    QNetworkRequest request = QNetworkRequest(QUrl(urlToGame));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &GameLauncher::LauncherDownloadProgress);

    // restart the application
}

void GameLauncher::GameDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qInfo() << "GameDownloadProgress";
    qInfo() << "bytesReceived " + QString::number(bytesReceived);
    qInfo() << "bytesTotal " + QString::number(bytesTotal);
    ui->progressBar->setValue(bytesReceived/bytesTotal);
}

void GameLauncher::LauncherDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qInfo() << "GameDownloadProgress";
    qInfo() << "bytesReceived " + QString::number(bytesReceived);
    qInfo() << "bytesTotal " + QString::number(bytesTotal);
}

void GameLauncher::LauncherDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "LauncherDownloadFinished";
    //qInfo() << reply->readAll();
    // Download the new version UnityGame.exe to tempGame.exe
    QByteArray newLauncherDownloadRaw = reply->readAll();
    QFile newLauncherDownloadFile = QFile("Launcher.zip");
    newLauncherDownloadFile.open(QIODevice::WriteOnly);
    newLauncherDownloadFile.write(newLauncherDownloadRaw);
    newLauncherDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/tempLauncher.zip");

    // rename Launcher.exe to oldLauncher.exe
    QDir oldLauncherFile = QDir("C:/Users/joshm/OneDrive/Desktop/User/download_launcher");
    //oldLauncherFile.open(QIODevice::WriteOnly);
    oldLauncherFile.rename("C:/Users/joshm/OneDrive/Desktop/User/download_launcher", "C:/Users/joshm/OneDrive/Desktop/User/oldLauncher");

    // rename tempLauncher.exe to Launcher.exe
    newLauncherDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/Launcher.zip");

    // delete oldLauncher.exe version
    //oldLauncherFile.remove();

    // delete oldLauncher folder
//    QDir oldGameFolder = QDir("C:/Users/joshm/OneDrive/Desktop/User/oldLauncher");
//    oldGameFolder.removeRecursively();

    // Unzip
    QProcess p;
    QStringList params;
    QString pythonPath = "C:/Users/joshm/AppData/Local/Microsoft/WindowsApps/PythonSoftwareFoundation.Python.3.9_qbz5n2kfra8p0/python3.9.exe";
    QString pythonScript = "C:/Users/joshm/OneDrive/Desktop/unzipper.py";
    QString path_to_launcher_zip = "C:/Users/joshm/OneDrive/Desktop/User/Launcher.zip";
    QString path_to_destination = "C:/Users/joshm/OneDrive/Desktop/User";
    params << pythonScript << path_to_launcher_zip << path_to_destination;
    p.start(pythonPath, params);
    p.waitForFinished(-1);
    p.close();

    // Remove Zip file
    newLauncherDownloadFile.remove();

    // Save
    SaveLauncherVersionInSetting();

    // update version gui
    SetLauncherVersionNumberGUI();
    isDownloadedVersionLauncherNewer = false;

    // restart application
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void GameLauncher::GetNewerGameVersion()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameDownloadFinished);

    // Download the actual unity game
    //urlToGame = "C:/Users/joshm/OneDrive/Desktop/Server/UnityGame.exe";
    urlToGame = "http://localhost:5000/api/download_game";
    QNetworkRequest request = QNetworkRequest(QUrl(urlToGame));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &GameLauncher::GameDownloadProgress);
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

void GameLauncher::LoadSettings()
{
    qInfo() << "loadSettings";
    QSettings settings(COMPANY, APPLICATION);

    // GAME
    //settings.setValue("version_game", "0.0.0"); //DEBUGGIN
    gameVersionSystem = settings.value("version_game").toString();
    ui->versionGameLabel->setText(settings.value("version_game").toString());

    // LAUNCHER
    //settings.setValue("version_launcher", "0.0.0"); //DEBUGGIN
    launcherVersionSystem = settings.value("version_launcher").toString();
    ui->versionLauncherNumber->setText(settings.value("version_launcher").toString());
}

void GameLauncher::SaveGameVersionInSetting()
{
    qInfo() << "saveVersionInSetting";
    QSettings settings(COMPANY, APPLICATION);
    settings.setValue("version_game", gameVersionDownload);
}

void GameLauncher::SaveLauncherVersionInSetting()
{
    qInfo() << "SaveLauncherVersionInSetting";
    QSettings settings(COMPANY, APPLICATION);
    settings.setValue("version_launcher", launcherVersionDownload);
}

void GameLauncher::on_play_update_button_clicked()
{
    if(ui->play_update_button->text().toLower() == "update")
    {
        GetNewerGameVersion();
    }else{
        // Boot up the unity game
        QDesktopServices::openUrl(QUrl("file:///C:/Users/joshm/OneDrive/Desktop/User/PrimePianistBuilds/New Unity Project.exe"));
    }
}


// Set the game and launcher versions to the same as the version on the website
void GameLauncher::FetchGameVersionNumberFromWebsite()
{
    qInfo() << "FetchGameVersionNumberFromWebsite";
    qInfo() << &manager;
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);
    //urlToGameVersion = "C:/Users/joshm/OneDrive/Desktop/Server/version_game.txt";
    urlToGameVersion = "http://localhost:5000/api/version_game";
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(urlToGameVersion)));
    qInfo() << &manager;
    qInfo() << reply->error();
}

void GameLauncher::GameVersionDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameVersionDownloadFinished";
    gameVersionDownload = reply->readAll();
    QSettings settings(COMPANY, APPLICATION);
    qInfo() << "Game Version: " << gameVersionDownload;
    //settings.setValue("version_game", GetVersionOfGameOnWebsite());
    //SetGameVersionNumberGUI();


    disconnect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);

    emit GameVersionDownloaded();
}

QString GameLauncher::GetVersionOfGameOnWebsite()
{
    qInfo() << "GetVersionOfGameOnWebsite";
    return gameVersionDownload;
}


void GameLauncher::FetchLauncherVersionNumberFromWebsite()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::LauncherVersionDownloadFinished);
    //urlToLauncherVersion = "C:/Users/joshm/OneDrive/Desktop/Server/version_launcher.txt";
    urlToLauncherVersion = "http://localhost:5000/api/version_launcher";
    manager.get(QNetworkRequest(QUrl(urlToLauncherVersion)));
}

QString GameLauncher::GetVersionOfLauncherOnWebsite()
{
    qInfo() << "GetVersionOfLauncherOnWebsite";
    return launcherVersionDownload;
}

void GameLauncher::UpdateLauncherPrompt()
{
    qInfo() << "Update Launcher";
    // TODO: Add a prompt that the user can choose to update the launcher or not

    updateWindow.setModal(true);
    updateWindow.exec();
}



void GameLauncher::LauncherVersionDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "LauncherVersionDownloadFinished";
    launcherVersionDownload = reply->readAll();
    QSettings settings(COMPANY, APPLICATION);
    qInfo() << "Launcher Version: " << launcherVersionDownload;
    //settings.setValue("version_launcher", GetVersionOfLauncherOnWebsite());
    //SetLauncherVersionNumberGUI();

    disconnect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::LauncherVersionDownloadFinished);

    isLauncherVersionDownloadedNewer();
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

void GameLauncher::SetLauncherVersionNumberGUI()
{
    QSettings settings(COMPANY, APPLICATION);
    ui->versionLauncherNumber->setText(settings.value("version_launcher").toString());
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
