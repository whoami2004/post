#ifndef POSTER_H
#define POSTER_H

#include <QObject>
#include <QHttp>
#include <QUrl>
#include <QIODevice>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QBuffer>
#include <QByteArray>

std::ostream &operator <<(std::ostream &o, QString str);

class Poster :public QObject
{
    Q_OBJECT

public:
	Poster();

    void setUrl(QUrl url);
    void setContent(QIODevice *contentFile);
	void post(QByteArray script);
    bool error();

protected:
	QHttp http;
    QUrl _url;
    QIODevice *_contentFile;
	bool _error;
	int postid;

protected slots:
	void requestStarted(int id);
	void dataSendProgress(int done, int total);
    void responseHeaderReceived();
	void dataReadProgress(int done, int total);
	void requestFinished(int id, bool error);

signals:
	void done();
};

#endif // POSTER_H
