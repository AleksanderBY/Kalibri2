#ifndef TREI5B_GLOBAL_H
#define TREI5B_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TREI5B_LIBRARY)
#  define TREI5BSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TREI5BSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TREI5B_GLOBAL_H
