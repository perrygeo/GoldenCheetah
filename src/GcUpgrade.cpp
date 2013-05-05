/*
 * Copyright (c) 2013 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "GoldenCheetah.h"
#include "Settings.h"
#include "GcUpgrade.h"
#include <QDebug>

int 
GcUpgrade::upgrade(const QDir &home)
{ 
    // what was the last version? -- do we need to upgrade?
    int last = appsettings->cvalue(home.dirName(), GC_VERSION_USED, 0).toInt();
    if (!last || last < VERSION_LATEST) {

        // For now we always do the same thing
        // when we have some maturity with versions we may
        // choose to do different things
        if (last < VERSION_LATEST) {

            // 1. Delete old files
            QStringList oldfiles;
            oldfiles << "*.cpi";
            oldfiles << "*.bak";
            foreach (QString oldfile, home.entryList(oldfiles, QDir::Files)) {
                QFile old(QString("%1/%2").arg(home.canonicalPath()).arg(oldfile));
                old.remove();

            }

            // 2. Remove old CLucece 'index'
            QFile index(QString("%1/index").arg(home.canonicalPath()));
            if (index.exists()) {
                removeIndex(index);
            }

            // 3. Remove metricDBv3 - force rebuild including the search index
            QFile db(QString("%1/metricDBv3").arg(home.canonicalPath()));
            if (db.exists()) db.remove();
        }

        // set to latest so we don't repeat
        appsettings->setCValue(home.dirName(), GC_VERSION_USED, VERSION_LATEST);
    }
    return 0;
}

class FileUtil
{
    public:
        static bool removeDir(const QString &dirName) {

            bool result = true;
            QDir dir(dirName);
 
            if (dir.exists(dirName)) {
                foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
                    if (info.isDir()) {
                        result = FileUtil::removeDir(info.absoluteFilePath());

                    } else {

                        result = QFile::remove(info.absoluteFilePath());
                    }
 
                    if (!result) { return result; }

                }
                result = dir.rmdir(dirName);
            }
 
        return result;
    }
};

void
GcUpgrade::removeIndex(QFile &index)
{
    FileUtil::removeDir(index.fileName());
}