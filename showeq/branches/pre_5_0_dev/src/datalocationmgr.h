/*
 * datalocationmgr.h
 *
 *  ShowEQ Distributed under GPL
 *  http://www.sourceforge.net/projects/seq
 *
 *  Copyright 2003 Zaphod (dohpaz@users.sourceforge.net). 
 *
 */

#ifndef _DATALOCATIONMGR_H_
#define _DATALOCATIONMGR_H_

#include <qstring.h>
#include <qdir.h>
#include <qfileinfo.h>

class DataLocationMgr
{
 public:
  DataLocationMgr(const QString& homeSubDir);
  ~DataLocationMgr();
  bool setupUserDirectory();
  QFileInfo findExistingFile(const QString& subdir, const QString& filename,
			     bool caseSensitive = false, 
			     bool preferUser = true);
  QFileInfo findWriteFile(const QString& subdir, const QString& filename,
			  bool caseSensitive = true, bool preferUser = true);

 protected:
  QFileInfo findFile(const QString& dir1, const QString& dir2,
		     const QString& subdir, const QString& filename,
		     bool caseSensitive = false);
  QFileInfo findFile(const QDir& dir, const QString& filename, 
		     bool caseSensitive = false, bool writable = false);
  QFileInfo findWriteFile(const QString& dir1, const QString& dir2,
			  const QString& subdir, const QString& filename,
			  bool caseSensitive = false);
  QDir findOrMakeSubDir(const QString& dir, const QString& subdir);

  QString m_pkgData;
  QString m_userData;
};

#endif // _DATALOCATIONMGR_H_


