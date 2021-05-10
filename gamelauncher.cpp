#include "gamelauncher.h"
#include "ui_gamelauncher.h"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);

    LoadSettings();

    // Update the launcher first
    FetchLauncherVersionNumberFromWebsite();



}

GameLauncher::~GameLauncher()
{
    delete ui;
}

void GameLauncher::GameDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameDownloadFinished";
    qInfo() << reply->readAll();
    // Download the new version UnityGame.exe to tempGame.exe
    QByteArray newGameDownloadRaw = reply->readAll();
    QFile newGameDownloadFile = QFile("UnityGame.exe");
    newGameDownloadFile.open(QIODevice::WriteOnly);
    newGameDownloadFile.write(newGameDownloadRaw);
    newGameDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/tempGame.exe");

    // rename UnityGame.exe to oldGame.exe
    QFile oldGameFile = QFile("C:/Users/joshm/OneDrive/Desktop/User/UnityGame.exe");
    oldGameFile.open(QIODevice::WriteOnly);
    oldGameFile.rename("C:/Users/joshm/OneDrive/Desktop/User/oldGame.exe");

    // rename tempGame.exe to UnityGame.exe
    newGameDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/UnityGame.exe");

    // delete oldGame.exe version
    oldGameFile.remove();

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
    urlToGame = "C:/Users/joshm/OneDrive/Desktop/Server/Launcher.exe";
    manager.get(QNetworkRequest(QUrl::fromLocalFile(urlToGame)));

    // restart the application
}

void GameLauncher::LauncherDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "LauncherDownloadFinished";
    qInfo() << reply->readAll();
    // Download the new version UnityGame.exe to tempGame.exe
    QByteArray newLauncherDownloadRaw = reply->readAll();
    QFile newLauncherDownloadFile = QFile("Launcher.exe");
    newLauncherDownloadFile.open(QIODevice::WriteOnly);
    newLauncherDownloadFile.write(newLauncherDownloadRaw);
    newLauncherDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/tempLauncher.exe");

    // rename Launcher.exe to oldLauncher.exe
    QFile oldLauncherFile = QFile("C:/Users/joshm/OneDrive/Desktop/User/Launcher.exe");
    oldLauncherFile.open(QIODevice::WriteOnly);
    oldLauncherFile.rename("C:/Users/joshm/OneDrive/Desktop/User/oldLauncher.exe");

    // rename tempLauncher.exe to Launcher.exe
    newLauncherDownloadFile.rename("C:/Users/joshm/OneDrive/Desktop/User/Launcher.exe");

    // update settings for currently installed launcher version
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
    urlToGame = "C:/Users/joshm/OneDrive/Desktop/Server/UnityGame.exe";
    manager.get(QNetworkRequest(QUrl::fromLocalFile(urlToGame)));
}

void GameLauncher::UpdateDownloadUpdateButtonGUI()
{
    qInfo() << "updateDownloadUpdateButtonGUI";
    if(isDownloadedVersionNewer)
    {
        ui->play_update_button->setText("Update");
    }
    else
    {
        ui->play_update_button->setText("Play");
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
    }
}


// Set the game and launcher versions to the same as the version on the website
void GameLauncher::FetchGameVersionNumberFromWebsite()
{
    qInfo() << "FetchGameVersionNumberFromWebsite";
    qInfo() << &manager;
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);
    urlToGameVersion = "C:/Users/joshm/OneDrive/Desktop/Server/version_game.txt";
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl::fromLocalFile(urlToGameVersion)));
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
    urlToLauncherVersion = "C:/Users/joshm/OneDrive/Desktop/Server/version_launcher.txt";
    manager.get(QNetworkRequest(QUrl::fromLocalFile(urlToLauncherVersion)));
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








