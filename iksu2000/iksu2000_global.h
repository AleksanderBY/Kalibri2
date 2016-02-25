#ifndef IKSU2000_GLOBAL_H
#define IKSU2000_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IKSU2000_LIBRARY)
#  define IKSU2000SHARED_EXPORT Q_DECL_EXPORT
#else
#  define IKSU2000SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IKSU2000_GLOBAL_H
