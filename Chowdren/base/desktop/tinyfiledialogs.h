/*
tinyfiledialogs.h
unique header file of "tiny file dialogs" created [November 9, 2014]
Copyright (c) 2014 - 2015 Guillaume Vareille http://ysengrin.com
http://tinyfiledialogs.sourceforge.net

(changed for use in Chowdren, 2015)
 
- License -

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef TINYFILEDIALOGS_H
#define TINYFILEDIALOGS_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription ) ; /* NULL or "image files" */
	/* returns NULL on cancel */

char const * tinyfd_openFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription , /* NULL or "image files" */
    int const aAllowMultipleSelects ) ; /* 0 or 1 */
	/* in case of multiple files, the separator is | */
	/* returns NULL on cancel */

int tinyfd_messageBox (
    char const * const aTitle , /* "" */
    char const * const aMessage , /* "" may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) ; /* 0 for cancel/no , 1 for ok/yes */
  /* returns 0 for cancel/no , 1 for ok/yes */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* TINYFILEDIALOGS_H */
