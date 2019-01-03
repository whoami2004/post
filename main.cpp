#include "poster.h"
#include <iostream>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    QStringList args = app.arguments();
    if (args.size() == 1)
    {
        std::cout << "Use '-h' or '--help' to show help information." << std::endl
                  << "Use '-v' or '--version' to show the version." << std::endl;
        return 0;
    }
    if (args.size() == 2)
    {
        if (args[1] == "-h" || args[1] == "--help")
        {
            QFile file("/etc/post/help.info");
            if (!file.open(QIODevice::ReadOnly))
            {
                std::cerr << "Error: Cannot open /etc/post/help.info." << std::endl
                          << "Maybe the package has been damaged." << std::endl
                          << "Please try to reinstall it." << std::endl
                          << "You can also visit the website to see if there is an update." << std::endl;
                return -1;
            }
            QTextStream in(&file);
            std::cout << in.readAll();
            file.close();
            return 0;
        }
        if (args[1] == "-v" || args[1] == "--version")
        {
            QFile file("/etc/post/version.info");
            if (!file.open(QIODevice::ReadOnly))
            {
                std::cerr << "Error: Cannot open /etc/post/version.info." << std::endl
                          << "Maybe the package has been damaged." << std::endl
                          << "Please try to reinstall it." << std::endl
                          << "YOu can also visit the website to see if there is an update." << std::endl;
                return -1;
            }
            QTextStream in(&file);
            std::cout << in.readAll();
            file.close();
            return 0;
        }
        std::cerr << "Error: Invalid argument." << std::endl
                  << "Use '-h' or '--help' to show help information." << std::endl;
        return 1;
    }
    QStringList::iterator p;
    QString url;
    QString script;
    QString outputFile;
    quint16 port = 80;
    for (p=args.begin()+1;p<args.end();p++)
    {
        if (*p == "-u" || *p == "--url")
        {
            if (++p == args.end())
            {
                std::cerr << "Error: Invalid arugment." << std::endl
                          << "Use '-h' or '--help' to show help information." << std::endl;
                return 1;
            }
            url = *p;
            continue;
        }
        if (*p == "-p" || *p == "--port")
        {
            if (++p == args.end())
            {
                std::cerr << "Error: Invalid argument." << std::endl
                          << "Use '-h' or '--help' to show help information." << std::endl;
                return 1;
            }
            bool ok;
            port = p->toInt(&ok);
            if (!ok)
            {
                std::cerr << "Error: Invalid argument." << std::endl
                          << "Use '-h' or '--help' to show help information." << std::endl;
                return 1;
            }
        }
        if (*p == "-s" || *p == "--script")
        {
            if (++p == args.end())
            {
                std::cerr << "Error: Invalid argument." << std::endl
                          << "Use '-h' or '--help' to show help information." << std::endl;
                return 1;
            }
            script = *p;
            continue;
        }
        if (*p == "-o" || *p == "--output")
        {
            if (++p == args.end())
            {
                std::cerr << "Error: Invalid argument." << std::endl
                          << "Use '-h' or '--help' to show help information." << std::endl;
                return 1;
            }
            outputFile = *p;
            continue;
        }
        std::cerr << "Error: Invalid argument." << std::endl
                  << "Use '-h' or '--help' to show help information." << std::endl;
        return 1;
    }
    if (url.isEmpty())
    {
        std::cerr << "Error: Url not given." << std::endl
                  << "Use '-u' or '--url' to set url." << std::endl
                  << "Use '-h' or '--help' to show help document." << std::endl;
        return 2;
    }
    QFile oFile;
    if (outputFile.isEmpty())
    {
        oFile.setFileName(QObject::tr("post_%1").arg(QTime::currentTime().toString("HH_mm_ss")));
        if (!oFile.open(QIODevice::WriteOnly))
        {
            std::cerr << "Error: Cannot open " << oFile.fileName() << "." << std::endl;
            return -1;
        }
    }
    else
    {
        oFile.setFileName(outputFile);
        if (!oFile.open(QIODevice::WriteOnly))
        {
            std::cerr << "Error: Cannot open " << outputFile << "." << std::endl;
            return -1;
        }
    }
    Poster poster;
    QUrl theUrl;
    if (url.mid(0,7) != "http://")
        url = QObject::tr("http://%1").arg(url);
    theUrl.setUrl(url);
    theUrl.setPort(port);
    if (theUrl.path().isEmpty())
        theUrl.setPath(QObject::tr("/"));
    poster.setUrl(theUrl);
    poster.setContent(&oFile);
    QBuffer null;
    QByteArray nullArray;
    null.setBuffer(&nullArray);
    QEventLoop loop;
    QObject::connect(&poster,SIGNAL(done()),
                     &loop,SLOT(quit()));
    poster.post(QByteArray(script.toStdString().c_str()));
    loop.exec();
    if (poster.error())
        return 3;
    oFile.close();
    return 0;
}
