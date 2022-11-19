#pragma once

#include <iostream>
#include <string>
#include <sys/stat.h>

static std::string FONT_PATH = "pixeloid_font/";
static bool can_draw_text = false;
static bool check_dir_exists(std::string dir_name);
static std::string get_config_path();

#ifdef _WIN32
#include <Windows.h>
static bool check_dir_exists(std::string dir_name) {
  DWORD ftyp = GetFileAttributesA(dir_name.c_str());
  if (ftyp == INVALID_FILE_ATTRIBUTES)
    return false;

  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    return true;

  return false;
}
#include <stdlib.h>
static std::string get_config_path() {
  char *pvalue;
  size_t len;
  errno_t err = _dupenv_s(&pvalue, &len, "USERPROFILE");
  if (err)
    return "";
  return std::string(pvalue) + std::string("\\.config\\iv2\\");
}
#else
// UNTESTED
static bool check_dir_exists(std::string dir_name) {
  struct stat st;
  if (stat(dir_name.c_str()) == 0)
    if (st.st_mode & S_IFDIR != 0)
      return true;
  return false;
}
#include <stdlib.h>
static std::string get_config_path() {
  char *hpath;
  hpath = getenv("HOME");
  if (hpath == NULL) {
    return "";
  }

  return std::string(hpath);
}
#endif

static void test_path() {
  std::string config_path = get_config_path();
  if (check_dir_exists(config_path)) {
    FONT_PATH = std::string(config_path) + std::string("pixeloid_font\\");
    if (check_dir_exists(FONT_PATH)) {
      can_draw_text = true;
    } else {
      printf("ERROR: Cannot find '%s', thus we can't draw text\n\tDownload the "
             "'pixeloid_font' from 'https://github.com/unknown989/iv2' and put "
             "it on the displayed path below:\n\t\t'%s'\n",
             FONT_PATH.c_str(), 
             std::string( std::string(config_path)+std::string("pixeloid_font\\") ).c_str()
             );
      can_draw_text = false;
    }
  }
}