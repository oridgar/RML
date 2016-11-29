/*
 * stdarg.h
 *
 *  Created on: Nov 29, 2016
 *      Author: idgaro
 */

#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;
#define va_start(ap,parmn) (void)((ap) = (char*)(&(parmn) + 1))
#define va_end(ap) (void)((ap) = 0)
#define va_arg(ap, type) \
    (((type*)((ap) = ((ap) + sizeof(type))))[-1])

#endif /* INCLUDE_STDARG_H_ */
