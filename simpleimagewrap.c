#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "ini.h"

#define N 30000

typedef struct
{
    const char* mounter;
    const char* app;
    const char* args;
    const char* vcdDrive;
    const char* vcdPath;
    const char* dtType;
    const char* dtDrive;
    const char* dtPath;
    int sleep;
} configuration;

static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("application", "mounter")) {
        pconfig->mounter = strdup(value);
    }
    else if (MATCH("application", "executable")) {
        pconfig->app = strdup(value);
    }
    else if (MATCH("application", "arguments")) {
        pconfig->args = strdup(value);
    }
    else if (MATCH("application", "sleep")) {
        pconfig->sleep = atoi(value);
    }
    else if (MATCH("virtualclonedrive", "path")) {
        pconfig->vcdPath = strdup(value);
    }
    else if (MATCH("virtualclonedrive", "drive")) {
        pconfig->vcdDrive = strdup(value);
    }
    else if (MATCH("daemontools", "type")) {
        pconfig->dtType = strdup(value);
    }
    else if (MATCH("daemontools", "path")) {
        pconfig->dtPath = strdup(value);
    }
    else if (MATCH("daemontools", "drive")) {
        pconfig->dtDrive = strdup(value);
    }
    else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

char *remove_three(const char *filename) {
    size_t len = strlen(filename);
    char *newfilename = malloc(len-2);
    if (!newfilename) /* handle error */;
    memcpy(newfilename, filename, len-3);
    newfilename[len - 3] = 0;
    return newfilename;
}


int main (int argc, char **argv) {
   int                 rc;
   STARTUPINFO         si;
   PROCESS_INFORMATION pi;

   configuration       config;
   TCHAR               szFileName[MAX_PATH];
   static char         buffer[4096];
   char                *iniName;
   char                ini[3];
   char                *image;
   char                *dirsep;
   char                *path;
   char                *drive;
   int                 isvcd;

   dirsep = strrchr( argv[0], '\\' );
   if( dirsep != NULL ) {
      *dirsep = 0;
   }

   // change working directory
   if(chdir(argv[0]) != 0) {
      printf("failed to set working directory to %s\n", argv[0]);
   }

   // figure out ini name, and load it
   GetModuleFileName( NULL, szFileName, MAX_PATH );
   sprintf(buffer, "%s", basename(szFileName));
   iniName = remove_three(buffer);
   strcpy(ini, "ini");
   strcat(iniName, ini);

   config.mounter = strdup("");
   config.app     = strdup("");
   config.args    = strdup("");
   config.sleep = 0;

   if (ini_parse(iniName, handler, &config) < 0) {
      printf("Can't load '%s'\n", buffer);
      return 1;
   }

   char *foobar = "virtualclonedrive";
   if(strcmp(config.mounter, foobar) == 0) {
      isvcd = 1;
      path  = strdup(config.vcdPath);
      drive = strdup(config.vcdDrive);
      image = malloc(snprintf(NULL, 0, " -mount %s,\"%s\"", drive, argv[1]) + 1);
      sprintf(image, " -mount %s,\"%s\"", drive, argv[1]);
   }
   else {
      isvcd = 0;
      path  = strdup(config.dtPath);
      drive = strdup(config.dtDrive);
      image = malloc(snprintf(NULL, 0, " -mount %s,%s,\"%s\"", config.dtType, drive, argv[1]) + 1);
      sprintf(image, " -mount %s,%s,\"%s\"", config.dtType, drive, argv[1]);
   }

   // print out a summary
   printf("config\t%s\npath\t%s\ndrive\t%s\nimage\t%s\napp\t%s\nargs\t%s\nsleep\t%d\nmounter\t%s\n\n", iniName, path, drive, argv[1], config.app, config.args, config.sleep, config.mounter);
   fflush(stdout);

   // mount drive
   printf ("mounting \"%s\" to drive %s\n", argv[1], drive);
   fflush(stdout);

   memset (&si, 0, sizeof (si));
   si.cb = sizeof (si);
   rc = CreateProcess (path, image, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
   if (!rc) {
      printf ("failed to launch process: %s%s; Errorcode: %lu\n", path, image, GetLastError ());
      exit(1);
   }

   // sleep a given amount of time after loading the image, before running the executable
   sleep(config.sleep);

   // run the executable
   printf ("starting %s%s\n", config.app, image);
   fflush(stdout);

   realloc(image, snprintf(NULL, 0, " %s", config.args) + 1);
   sprintf(image, " %s", config.args);
   rc = CreateProcess (config.app, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
   if (!rc) {
      printf ("failed to launch process: %s%s; Errorcode: %lu\n", config.app, image, GetLastError ());
      exit(1);
   }

   // wait for the executable to finish
   printf ("waiting for application to exit\n");
   fflush(stdout);
   WaitForSingleObject(pi.hProcess, INFINITE);

   // unmount the image we mounted earlier
   printf ("unmounting drive %s\n", drive);
   fflush(stdout);
   if(isvcd == 1) {
      realloc(image, snprintf(NULL, 0, " -unmount %s", drive) + 1);
      sprintf(image, " -unmount %s", drive);
   }
   else {
      realloc(image, snprintf(NULL, 0, " -unmount %s,%s", config.dtType, drive) + 1);
      sprintf(image, " -unmount %s,%s", config.dtType,drive);
   }
   rc = CreateProcess (path, image, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
   if (!rc) {
      printf ("failed to launch process: %s%s; Code %lu\n", path, image, GetLastError ());
      exit(1);
   }

   // all done
   return 0;
}
