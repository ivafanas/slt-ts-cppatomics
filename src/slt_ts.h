#ifndef SLT_TS_CPPATOMICS_SLT_TS_H
#define SLT_TS_CPPATOMICS_SLT_TS_H

#if defined(__clang__) || defined(__GNUC__)
  #define SLT_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define SLT_PRETTY_FUNCTION __FUNCSIG__
#else
  #define SLT_PRETTY_FUNCTION __FUNCTION__
#endif

#endif // SLT_TS_CPPATOMICS_SLT_TS_H
