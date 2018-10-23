// Copyright 2018 Hunter L. Allen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FILE_DOWNLOADER_HPP_
#define FILE_DOWNLOADER_HPP_
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QObject>

class FileDownloader : public QObject
{
  Q_OBJECT

public:
  explicit FileDownloader(QUrl imageUrl, QObject * parent = 0);
  virtual ~FileDownloader();
  QByteArray * downloadedData() const;
  QString get_url() const;

  Q_SIGNAL void downloaded();

private:
  Q_SLOT void fileDownloaded(QNetworkReply * pReply);
  QNetworkAccessManager m_WebCtrl;
  QByteArray * m_DownloadedData;
  QString m_url;
};
#endif  // FILE_DOWNLOADER_HPP_
