/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */

#ifndef UTILS_BASE64_H
#define UTILS_BASE64_H

#include <string>

#include "localconsts.h"

unsigned char *php3_base64_encode(const unsigned char *restrict,
                                  int, int *restrict) A_WARN_UNUSED;
unsigned char *php3_base64_decode(const unsigned char *restrict,
                                  int, int *restrict ) A_WARN_UNUSED;

std::string encodeBase64String(std::string value) A_WARN_UNUSED;

std::string decodeBase64String(std::string value) A_WARN_UNUSED;

#endif  // UTILS_BASE64_H
