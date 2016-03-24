#ifndef MSR63_GLOBAL_H
#define MSR63_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSR63_LIBRARY)
#  define MSR63SHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSR63SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSR63_GLOBAL_H
