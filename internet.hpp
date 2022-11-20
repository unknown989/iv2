#include "string.h"
#include <Windows.h>
#include <corecrt_wstdio.h>
#include <cwchar>
#include <iostream>
#include <stdio.h>
#include <wininet.h> // from SDK


static int DownloadURLImage(TCHAR *szURL, TCHAR *szpath, TCHAR *szname);



#ifdef _WIN32
// https://stackoverflow.com/a/11973893/14762883
static int convertURLtofname(TCHAR *szURL, TCHAR *szname)
// extract the filename from the URL
{
  char aszfilename[100];
  HRESULT result;
  char achar[3], aszURL[100];
  size_t nchars, i, j;
  int fresult;

  fresult = 0;
  nchars = strlen(szURL);
  i = nchars - 1;
  while ((i > 0) && (szURL[i] != '/') && (szURL[i] != '\\')) {
    i--;
  }
  j = 0;
  i++;
  while (i < nchars) {
    szname[j++] = szURL[i++];
  }
  szname[j] = '\0';

  //  wcstombs ( aszfilename, szname, 100 );
  //     cout << aszfilename << endl;
  //----------------------------------------------
  return fresult;
}

static int determinepathfilename(TCHAR *szURL, TCHAR *szpath, TCHAR *szname,
                          TCHAR *szpathfilename) {
  // use path and filename when supplied.  If filename (e.g. funkypic.jpg) is
  // not supplied, then the filename will be extracted from the last part of the
  // URL
  int result;
  result = 0;
  TCHAR szname_copy[100];

  if ((szname == NULL) || (szname[0] == '\0'))
    convertURLtofname(szURL, szname_copy);
  else
    strcpy_s(szname_copy, szname);

  if ((szpath == NULL) || (szpath[0] == '\0'))
    strcpy(szpathfilename, szname_copy);
  else {
    strcpy(szpathfilename, szpath);
    strcat(szpathfilename, szname_copy);
  }
  return result;
}

static bool GetFile(HINTERNET hOpen, // Handle from InternetOpen()
             TCHAR *szURL,    // Full URL
             TCHAR *szpath, TCHAR *szname) {
  DWORD dwSize;
  TCHAR szHead[15];
  BYTE *szTemp[1024];
  HINTERNET hConnect;
  FILE *pFile;
  TCHAR szpathfilename[100];

  szHead[0] = '\0';

  if (!(hConnect = InternetOpenUrl(hOpen, szURL, szHead, 15,
                                   INTERNET_FLAG_DONT_CACHE, 0))) {
    std::cout << "Error: InternetOpenUrl" << std::endl;
    return 0;
  }

  determinepathfilename(szURL, szpath, szname, szpathfilename);

  if (!(pFile = fopen(szpathfilename, "wb"))) {
    std::cerr << "Error _tfopen" << std::endl;
    return false;
  }
  do {
    // Keep copying in 1024 bytes chunks, while file has any data left.
    // Note: bigger buffer will greatly improve performance.
    if (!InternetReadFile(hConnect, szTemp, 1024, &dwSize)) {
      fclose(pFile);
      std::cerr << "Error InternetReadFile" << std::endl;
      return FALSE;
    }
    if (!dwSize)
      break; // Condition of dwSize=0 indicate EOF. Stop.
    else
      fwrite(szTemp, sizeof(BYTE), dwSize, pFile);
  } // do
  while (TRUE);
  fflush(pFile);
  fclose(pFile);

  return TRUE;
}

static int DownloadURLImage(TCHAR *szURL, TCHAR *szpath, TCHAR *szname) {
  int result;

  HINTERNET hInternet;

  result = 0;

  hInternet = InternetOpen("imaged",
                           INTERNET_OPEN_TYPE_DIRECT, //__in  DWORD dwAccessType
                           NULL, //__in  LPCTSTR lpszProxyName,
                           NULL, //__in  LPCTSTR lpszProxyBypass,
                           0  //_in   DWORD dwFlags
  );

  GetFile(hInternet, szURL, szpath, szname);
  InternetCloseHandle(hInternet);
  return result;
}


#endif
