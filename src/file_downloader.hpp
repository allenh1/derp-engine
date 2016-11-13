#ifndef __FILEDOWNLOADER_HPP__
#define __FILEDOWNLOADER_HPP__
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QObject>

class FileDownloader : public QObject
{
	Q_OBJECT
public:
	explicit FileDownloader(QUrl imageUrl, QObject *parent = 0);
	virtual ~FileDownloader();
	QByteArray downloadedData() const;
	QString get_url() const;
	
	Q_SIGNAL void downloaded();
	
private:
	Q_SLOT void fileDownloaded(QNetworkReply* pReply);
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
	QString m_url;
};
#endif

