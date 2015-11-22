//
//  Prefix header
//
//  The contents of this file should be included at the beginning of every source file.
//

#define GCD

#ifdef __OBJC__
  #import <Foundation/Foundation.h>
  #import <AppKit/AppKit.h>
#endif

#ifdef GCD
  #include <dispatch/dispatch.h>
#endif
