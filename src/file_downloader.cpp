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

#include <derp-engine/file_downloader.hpp>

FileDownloader::FileDownloader(QUrl _url, QObject * parent)
: QObject(parent)
{
  connect(
    &m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(fileDownloaded(QNetworkReply*))
  );

  QNetworkRequest request(_url);
  m_url = _url.toString();
  m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() {}

void FileDownloader::fileDownloaded(QNetworkReply * pReply)
{
  m_DownloadedData = new QByteArray(pReply->readAll());
  pReply->deleteLater();
  Q_EMIT (downloaded());
}

QByteArray * FileDownloader::downloadedData() const
{
  return m_DownloadedData;
}

QString FileDownloader::get_url() const {return m_url;}
