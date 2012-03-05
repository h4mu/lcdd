#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <signal.h>

#include "lcd.h"

static inline struct timeval waitms(long millisecs)
{
//  fd_set dummy;
  struct timeval toWait;

//  FD_ZERO(&dummy);
  toWait.tv_sec = millisecs / 1000;
  toWait.tv_usec = (millisecs % 1000) * 1000;

  return toWait; //select(0, &dummy, 0, 0, &toWait);
}

std::string text;
pthread_mutex_t textmutex = PTHREAD_MUTEX_INITIALIZER;

static void* gettext(void*)
{
  const size_t BUFLEN = 512;
  char buf[BUFLEN];
  std::string tmp;
  size_t len = 0;
  while(true)
  {
    pthread_testcancel();
    FILE* f = popen("/usr/bin/mocp -Q '%title'", "r");
    if(!f || ferror(f) || feof(f))
      throw std::exception();
    do
    {
      len = fread(buf, sizeof(buf[0]), BUFLEN, f);
      tmp.append(buf, len);
    }
    while(!feof(f) && !ferror(f) && len == BUFLEN);
    fclose(f);
    pthread_testcancel();
    tmp[tmp.length() - 1] = ' ';
    pthread_mutex_lock(&textmutex);
    text = tmp;
    pthread_mutex_unlock(&textmutex);
    tmp.clear();
    waitms(1200);
  }
}

static inline bool execute(const std::string & cmd, const std::string & path)
{
  return !cmd.empty() && !fork() && !execl("/usr/bin/mocp", ("-" + cmd + " " + path).c_str());
}

int main(int argc, char **argv)
{
  pthread_t thread;

  {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &sa, NULL);
  }

  if(pthread_create(&thread, NULL, gettext, NULL))
    throw std::exception();

  LCD lcd;
  while(true)
  {
    waitms(150);
    pthread_mutex_lock(&textmutex);
    lcd.bufferedUpdate(text);
    pthread_mutex_unlock(&textmutex);
  }

  pthread_cancel(thread);
  
  lcd.print("");
  return EXIT_SUCCESS;
}
