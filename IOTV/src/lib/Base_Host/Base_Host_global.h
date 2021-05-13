#ifndef BASE_HOST_GLOBAL_H
#define BASE_HOST_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BASE_HOST_LIBRARY)
#  define BASE_HOST_EXPORT Q_DECL_EXPORT
#else
#  define BASE_HOST_EXPORT Q_DECL_IMPORT
#endif

#endif // BASE_HOST_GLOBAL_H
