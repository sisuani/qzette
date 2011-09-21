/*
 *
 *
 */

#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QString>

class ApplicationInfo
{

public:
    // Version info
    static QString name();
    static QString version();
    static QString build();

    // Directories
    static QString homeDir();
    static QString resourcesDir();
    static QString tmpDir();
    static QString homePath();
    static QString configFilePath();
    static QString logDirPath();
    static QString imagesAppPath();

};

#endif /* APPLICATIONINFO_H */
