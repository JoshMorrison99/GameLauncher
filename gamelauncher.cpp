#include "gamelauncher.h"
#include "ui_gamelauncher.h"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);


    LoadSettings();
    if(isFirstInstall())
    {
        SetupFirstInstall();
    }

    //debug
    gameVersionSystem = "0.0.0";


    GetVersionOfGameOnWebsite();
    connect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);
    UpdateVersionNumberGUI();
}

GameLauncher::~GameLauncher()
{
    delete ui;
}

void GameLauncher::GetVersionOfGameOnWebsite()
{
    qInfo() << "getVersionOfGameOnWebsite";
    urlToGameVersion = "C:/Users/joshm/OneDrive/Desktop/Server/version_game.txt";
    manager.get(QNetworkRequest(QUrl::fromLocalFile(urlToGameVersion)));
}

void GameLauncher::UpdateVersionNumberGUI()
{
    QSettings settings(COMPANY, APPLICATION);
    ui->versionGameLabel->setText(settings.value("version_game").toString());
}

void GameLauncher::GameVersionDownloadFinished(QNetworkReply *reply)
{
    qInfo() << "GameVersionDownloadFinished";
    gameVersionDownload = reply->readAll();
    UpdateDownloadUpdateButtonGUI();

    disconnect(&manager, &QNetworkAccessManager::finished, this, &GameLauncher::GameVersionDownloadFinished);
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


}

QStringList GameLauncher::ParseVersionString(QString version)
{
    qInfo() << "parseVersionString";
    version = version.trimmed();
    QStringList versionNumbers = version.split(".");
    return versionNumbers;
}

bool GameLauncher::isGameVersionDownloadedNewer()
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
        return false;
    }
    return true;
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
    if(isGameVersionDownloadedNewer())
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
    qInfo() << settings.value("version_game").toString();
    return false;
}

void GameLauncher::SetupFirstInstall()
{
    qInfo() << COMPANY;
    qInfo() << APPLICATION;
    QSettings settings(COMPANY, APPLICATION);
    settings.setValue("version_game", "0.0.0");
    settings.setValue("version_launcher", "0.0.0");
    gameVersionSystem = settings.value("version_game", "0.0.0").toString();
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




