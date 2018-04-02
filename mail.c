/*
   04/02/2018 https://github.com/su8/query-gmail

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <curl/curl.h>

static size_t read_gmail_data_cb(char *, size_t size, size_t nmemb, char *);

static size_t
read_gmail_data_cb(char *data, size_t size, size_t nmemb, char *str1) {
  char *ptr = data;
  size_t sz = nmemb * size, x = 0;

  for (; *ptr; ptr++, x++) {
    if ((x+17) < sz) { /* Verifying up to *(ptr+17) */

      if ('f' == *ptr) { /* fullcount */
        if ('u' == *(ptr+1) && 'l' == *(ptr+2) && 'l' == *(ptr+3)) {
          *str1++ = *(ptr+10); /* 1 email */
          if (0 != (isdigit((unsigned char) *(ptr+11)))) {
            *str1++ = *(ptr+11); /* 10 emails */
          }
          if (0 != (isdigit((unsigned char) *(ptr+12)))) {
            *str1++ = *(ptr+12); /* 100 emails */
          }
          if (0 != (isdigit((unsigned char) *(ptr+13)))) {
            *str1++ = *(ptr+13); /* 1000 emails */
          }
          if (0 != (isdigit((unsigned char) *(ptr+14)))) {
            *str1++ = *(ptr+14); /* 10 000 emails */
          }
          if (0 != (isdigit((unsigned char) *(ptr+15)))) {
            *str1++ = *(ptr+15); /* 100 000 emails */
          }
          *str1 = '\0';
          break;
        }
      }

    }
  }

  if ('\0' != *str1) {
    *str1++ = '\0';
  }
  return sz;
}


int main(int argc, char *argv[]) {
  char str[200] = "0";
  const char *const da_url = "https://mail.google.com/mail/feed/atom";

  CURL *curl = NULL;
  CURLcode res;

  if (3 > argc) {
    printf("%s\n", "Missing account or password");
    return EXIT_FAILURE;
  }

  const char *const GMAIL_ACC  = argv[1];
  const char *const GMAIL_PASS = argv[2];

  curl_global_init(CURL_GLOBAL_ALL);

  if (NULL == (curl = curl_easy_init())) {
    goto error;
  }

  curl_easy_setopt(curl, CURLOPT_USERNAME, GMAIL_ACC);
  curl_easy_setopt(curl, CURLOPT_PASSWORD, GMAIL_PASS);
  curl_easy_setopt(curl, CURLOPT_URL, da_url);
  curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
  curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL); 
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read_gmail_data_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);

  res = curl_easy_perform(curl);
  if (CURLE_OK != res) {
    goto error;
  }

error:
  if (NULL != curl) {
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  printf("Unread Mails %s\n", str);
  return EXIT_SUCCESS;
}
