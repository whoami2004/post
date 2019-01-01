#include "poster.h"
#include <iostream>
#include <QDateTime>

std::ostream &operator <<(std::ostream &o, QString str)
{return o << qPrintable(str);}

Poster::Poster()
{
    connect(&http,SIGNAL(requestStarted(int)),
            this,SLOT(requestStarted(int)));
	connect(&http,SIGNAL(dataSendProgress(int,int)),
			this,SLOT(dataSendProgress(int,int)));
    connect(&http,SIGNAL(responseHeaderReceived(QHttpResponseHeader)),
            this,SLOT(responseHeaderReceived()));
    connect(&http,SIGNAL(dataReadProgress(int,int)),
            this,SLOT(dataReadProgress(int,int)));
    connect(&http,SIGNAL(requestFinished(int,bool)),
            this,SLOT(requestFinished(int,bool)));
    connect(&http,SIGNAL(done(bool)),
            this,SIGNAL(done()));
}

void Poster::setUrl(QUrl url)
{
    _url = url;
    http.setHost(_url.host(),_url.port());
}
void Poster::setContent(QIODevice *contentFile)
{_contentFile = contentFile;}
void Poster::post(QByteArray script)
{
    contentBuffer.setBuffer(&_content);
    postid = http.post(_url.path(),script,&contentBuffer);
}
bool Poster::error()
{return _error;}
QByteArray Poster::content()
{return _content;}

void Poster::requestStarted(int id)
{
    if (id != postid)
        return;
    std::cout << "Connecting to the host ("
        << _url.host() << ") ..." << std::endl;
    _error = false;
}
void Poster::dataSendProgress(int done, int total)
{
    if (total)
        std::cout << 100*done/total << "% ";
    std::cout << "[ Data sending ";
    if (done < 10240)
        std::cout << done << "B";
    else if (done/1024 < 10240)
        std::cout << done/1024 << "KB";
    else
        std::cout << done/1024/1024 << "MB";
    if (total)
    {
        std::cout << "/";
        if (total < 10240)
            std::cout << total << "B ";
        else if (total/1024 < 10240)
            std::cout << total/1024 << "KB ";
        else
            std::cout << total/1024/1024 << "MB ";
    }
    static uint dt = QDateTime::currentDateTime().toTime_t();
    std::cout << QDateTime::currentDateTime().toTime_t() - dt << "s ]     \r";
}
void Poster::responseHeaderReceived()
{
    std::cout << std::endl << "Data sending completed." << std::endl;
}
void Poster::dataReadProgress(int done, int total)
{
    if (total)
        std::cout << (qint64)100*done/total << "% ";
    std::cout << "[ Data reading ";
    if (done < 10240)
        std::cout << done << "B";
    else if (done/1024 < 10240)
        std::cout << done/1024 << "KB";
    else
        std::cout << done/1024/1024 << "MB";
    if (total)
    {
        std::cout << "/";
        if (total < 10240)
            std::cout << total << "B ";
        else if (total/1024 < 10240)
            std::cout << total/1024 << "KB ";
        else
            std::cout << total/1024/1024 << "MB ";
    }
    static uint dt = QDateTime::currentDateTime().toTime_t();
    std::cout << QDateTime::currentDateTime().toTime_t() - dt << "s ]     \r";
}
void Poster::requestFinished(int id, bool error)
{
    if (id != postid)
        return;
    std::cout << std::endl;
    if (error)
    {
        std::cerr << std::endl << "Error: ";
        switch (http.error())
        {
        case QHttp::HostNotFound: std::cerr << "Host not found.";break;
        case QHttp::ConnectionRefused: std::cerr << "Connection refused.";break;
        case QHttp::UnexpectedClose: std::cerr << "Connection unexpectedly closed.";break;
        case QHttp::InvalidResponseHeader: std::cerr << "Invalid response header.";break;
        case QHttp::WrongContentLength: std::cerr << "Wrong content length.";break;
        case QHttp::Aborted: std::cerr << "Request aborted.";break;
        case QHttp::ProxyAuthenticationRequiredError: std::cerr << "Proxy authentication required.";break;
        case QHttp::AuthenticationRequiredError: std::cerr << "Authentication required.";break;
        default: std::cerr << "Unknown error.";break;
        }
        std::cerr << std::endl;
        _error = true;
        return;
    }
    std::cout << "Request completed." << std::endl
              << std::endl
              << "Outputing ..." << std::endl
              << std::endl;
    _error = false;
    _contentFile->write(_content);
    _contentFile->write("\n");
}
