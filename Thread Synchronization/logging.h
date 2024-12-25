/*
 * logging.h
 *
 * UVic CSC 360, Summer 2023
 *
 * DO NOT MODIFY THIS SOURCE-CODE FILE 
 */
#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOG_MAX_LINES 100
#define LOG_MAX_LINE_LEN 80

char kosmos_log[LOG_MAX_LINES][LOG_MAX_LINE_LEN];
unsigned char kosmos_log_numlines;

void kosmos_log_init(void);
void kosmos_log_dump(void);
void kosmos_log_add_entry(int, int, int, int, char *);
void kosmos_log_add_line(char *);

#endif
