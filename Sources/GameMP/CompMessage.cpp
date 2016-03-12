/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdAfx.h"

#include "CompMessage.h"
extern CTString _strStatsDetails;

CCompMessage::CCompMessage(void)
{
  Clear();
}
void CCompMessage::Clear(void)
{
  UnprepareMessage();
  cm_fnmFileName.Clear();
  cm_pcmiOriginal = NULL;
  cm_bRead = FALSE;
}

// constructs message with a filename
void CCompMessage::SetMessage(CCompMessageID *pcmi)
{
  cm_fnmFileName = pcmi->cmi_fnmFileName;
  cm_bRead = pcmi->cmi_bRead;
  cm_pcmiOriginal = pcmi;
}

// load a message from file
void CCompMessage::Load_t(void)
{
  // if already loaded
  if (cm_bLoaded) {
    // do nothing
    return;
  }
  // open file
  CTFileStream strm;
  strm.Open_t(cm_fnmFileName);
  // read subject line
  strm.ExpectKeyword_t("SUBJECT\r\n");
  strm.GetLine_t(cm_strSubject);
  // rea image type
  strm.ExpectKeyword_t("IMAGE\r\n");
  CTString strImage;
  strm.GetLine_t(strImage);
  if (strImage=="none") {
    cm_itImage = IT_NONE;
  } else if (strImage=="statistics") {
    cm_itImage = IT_STATISTICS;
  } else if (strImage=="picture") {
    cm_itImage = IT_PICTURE;
    cm_fnmPicture.ReadFromText_t(strm);
  } else if (strImage=="model") {
    cm_itImage = IT_MODEL;
    cm_strModel.ReadFromText_t(strm, "");
  } else {
    throw TRANS("Unknown image type!");
  }
  // read text until end of file
  strm.ExpectKeyword_t("TEXT\r\n");
  cm_strText.ReadUntilEOF_t(strm);
  cm_ctFormattedWidth = 0;
  cm_ctFormattedLines = 0;
  cm_strFormattedText = "";
  cm_bLoaded = TRUE;
}

// format message for given line width
void CCompMessage::Format(INDEX ctCharsPerLine)
{
  // if already formatted in needed size
  if (cm_ctFormattedWidth == ctCharsPerLine) {
    // do nothing
    return;
  }
  // remember width
  cm_ctFormattedWidth = ctCharsPerLine;

  // get text
  const char *strText = cm_strText;
  if (strncmp(strText, "$STAT", 5)==0) {
    strText = _strStatsDetails;
    cm_strFormattedText = strText;
    cm_ctFormattedLines = 1;
    for (INDEX i=0; i<cm_strFormattedText.Length(); i++) {
      if (cm_strFormattedText[i]=='\n') {
        cm_ctFormattedLines++;
      }
    }
    return;
  }

  // allocate overestimated buffer
  SLONG slMaxBuffer = strlen(strText)*2;
  char *pchBuffer = (char *)AllocMemory(slMaxBuffer);

  // start at the beginning of text and buffer
  const char *pchSrc = strText;
  char *pchDst = pchBuffer;
  cm_ctFormattedLines = 1;
  INDEX ctChars = 0;
  // while not end of text
  while(*pchSrc!=0) {
    // copy one char
    char chLast = *pchDst++ = *pchSrc++;
    // if it was line break
    if (chLast=='\n') {
      // new line
      ctChars=0;
      cm_ctFormattedLines++;
      continue;
    }
    ctChars++;
    // if out of row
    if (ctChars>ctCharsPerLine) {
      // start backtracking
      const char *pchSrcBck = pchSrc-1;
            char *pchDstBck = pchDst-1;
      // while not start of row and not space
      while (pchSrcBck>pchSrc-ctChars && *pchSrcBck!=' ') {
        // go one char back
        pchSrcBck--;
        pchDstBck--;
      }
      // if start of row hit (cannot word-wrap)
      if (pchSrcBck<pchSrc-ctChars) {
        // just go to next line
        pchSrc--;
        pchDst--;
        *pchDst++='\n';
        ctChars=0;
        cm_ctFormattedLines++;
        continue;
      }
      // if can word-wrap, insert break before the last word
      pchSrc = pchSrcBck+1;
      pchDst = pchDstBck;
      *pchDst++='\n';
      ctChars=0;
      cm_ctFormattedLines++;
    }
  }

  // add end marker
  *pchDst=0;

  cm_strFormattedText = pchBuffer;
  FreeMemory(pchBuffer);
}

// prepare message for using (load, format, etc.)
void CCompMessage::PrepareMessage(INDEX ctCharsPerLine)
{
  // if not loaded
  if (!cm_bLoaded) {
    // try to
    try {
      // load it
      Load_t();
    // if failed
    } catch (char *strError) {
      // report warning
      CPrintF("Cannot load message'%s': %s\n", (const CTString &)cm_fnmFileName, strError);
      // do nothing else
      return;
    }
  }

  // format it for new width
  Format(ctCharsPerLine);
}

// free memory used by message, but keep message filename
void CCompMessage::UnprepareMessage(void)
{
  // clear everything except filename
  cm_bLoaded = FALSE;
  cm_strSubject.Clear();
  cm_strText.Clear();
  cm_strModel.Clear();
  cm_fnmPicture.Clear();
  cm_itImage = IT_NONE;
  cm_strFormattedText.Clear();
  cm_ctFormattedWidth = 0;
  cm_ctFormattedLines = 0;
}
// mark message as read
void CCompMessage::MarkRead(void)
{
  cm_bRead = TRUE;
  cm_pcmiOriginal->cmi_bRead = TRUE;
}

// get one formatted line
CTString CCompMessage::GetLine(INDEX iLine)
{
  const char *strText = cm_strFormattedText;
  // find first line
  INDEX i = 0; 
  while (i<iLine) {
    strText = strchr(strText, '\n');
    if (strText==NULL) {
      return "";
    } else {
      i++;
      strText++;
    }
  }
  // find end of line
  CTString strLine = strText;
  char *pchEndOfLine = (char*)strchr(strLine, '\n');
  // if found
  if (pchEndOfLine!=NULL) {
    // cut there
    *pchEndOfLine = 0;
  }
  return strLine;
}
