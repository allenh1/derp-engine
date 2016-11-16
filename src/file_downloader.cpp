#include "file_downloader.hpp"
 
FileDownloader::FileDownloader(QUrl _url, QObject *parent) :
	QObject(parent)
{
	connect(
		&m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
		this, SLOT (fileDownloaded(QNetworkReply*))
		);
 
	QNetworkRequest request(_url);
	m_url = _url.toString();
	m_WebCtrl.get(request);
}
 
FileDownloader::~FileDownloader() { }
 
void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
	m_DownloadedData = new QByteArray(pReply->readAll());
	pReply->deleteLater();
	Q_EMIT(downloaded());
}
 
QByteArray* FileDownloader::downloadedData() const {
	return m_DownloadedData;
}

QString FileDownloader::get_url() const { return m_url; }
