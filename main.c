#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>

static int write_header(FILE *file)
{
	time_t cur_time;
	struct tm *the_time;
	time(&cur_time);
	the_time = localtime(&cur_time);
	if (the_time == NULL) {
		return -1;
	}

	fprintf(file, "<background>\n");
	fprintf(file, "  <starttime>\n");
	fprintf(file, "    <year>%04d</year>\n", the_time->tm_year + 1900);
	fprintf(file, "    <month>%02d</month>\n", the_time->tm_mon + 1);
	fprintf(file, "    <day>%02d</day>\n", the_time->tm_mday);
	fprintf(file, "    <hour>%02d</hour>\n", the_time->tm_hour);
	fprintf(file, "    <minute>00</minute>\n");
	fprintf(file, "    <second>00</second>\n");
	fprintf(file, "  </starttime>\n");

	return 0;
}

static int write_tail(FILE *file)
{
	fprintf(file, "</background>");

	return 0;
}

static int write_content(char *dir, FILE *file, int recur, int keep, int tran)
{
	int ret = -1;
	int cnt = 0;
	DIR *cur_dir;
	struct dirent *pdir;
	cur_dir = opendir(dir);
	if (cur_dir == NULL) {
		return -1;
	}

	char *first = (char *)malloc(sizeof(char) * 65536);
	if (first == NULL) {
		ret = -1;
		goto first_err;
	}
	char *tmp = (char *)malloc(sizeof(char) * 65536);
	if (tmp == NULL) {
		ret = -1;
		goto tmp_err;
	}

	strcpy(first, "");
	strcpy(tmp, "");

	while ((pdir = readdir(cur_dir)) != NULL) {
		if (strcmp(pdir->d_name, ".") == 0 || strcmp(pdir->d_name, "..") == 0) {
			continue;
		} else if (pdir->d_type == DT_DIR) {
			if (recur == 0) {
				continue;
			} else {

			}
		} else if (pdir->d_type == DT_REG) {
			cnt++;
			if (cnt == 1) {
				sprintf(first, "%s/%s", dir, pdir->d_name);
			}
			if (cnt > 1) {
				fprintf(file, "  <transition>\n");
				fprintf(file, "    <duration>%d</duration>\n", tran);
				fprintf(file, "    <from>%s</from>\n", tmp);
				fprintf(file, "    <to>%s/%s</to>\n", dir, pdir->d_name);
				fprintf(file, "  </transition>\n");
			}
			fprintf(file, "  <static>\n");
			fprintf(file, "    <duration>%d</duration>\n", keep);
			fprintf(file, "    <file>%s/%s</file>\n", dir, pdir->d_name);
			fprintf(file, "  </static>\n");

			sprintf(tmp, "%s/%s", dir, pdir->d_name);
		}
	}

	fprintf(file, "  <transition>\n");
	fprintf(file, "    <duration>%d</duration>\n", tran);
	fprintf(file, "    <from>%s</from>\n", tmp);
	fprintf(file, "    <to>%s</to>\n", first);
	fprintf(file, "  </transition>\n");

	ret = 0;

	if (tmp != NULL) {
		free(tmp);
		tmp = NULL;
	}
tmp_err:
	if (first != NULL) {
		free(first);
		first = NULL;
	}

first_err:
	closedir(cur_dir);

	return ret;
}

static int wac(char *in_dir, char *out_file, int recur, int keep, int tran)
{
	int ret = -1;
	FILE *of = fopen(out_file, "w");
	if (of == NULL) {
		return ret;
	}

	write_header(of);
	write_content(in_dir, of, recur, keep, tran);
	write_tail(of);

	fclose(of);
	return ret;
}

static void usage()
{
	printf("Usage: wac -i[input directory] -o[output file] [-r] -k [keep time] -t [tran time]\n");
}

int main(int argc, char **argv)
{
	int ret = -1;
	int opt;
	int optlen;
	char *in_dir = NULL;
	char *out_file = NULL;
	int recur = 0;
	int keep = 1795;
	int tran = 5;

	if (argc < 5) {
		usage();
		goto done;
	}
	while ((opt = getopt(argc, argv, "i:o:rk:t:")) != -1) {
		switch (opt) {
		case 'i':
			optlen = strlen(optarg);
			in_dir = (char *)malloc(sizeof(char) * optlen + 1);
			if (in_dir == NULL) {
				ret = -1;
				goto indir_err;
			}
			strcpy(in_dir, optarg);
			break;
		case 'o':
			optlen = strlen(optarg);
			out_file = (char *)malloc(sizeof(char) * optlen + 1);
			if (out_file == NULL) {
				ret = -1;
				goto outfile_err;
			}
			strcpy(out_file, optarg);
			break;
		case 'r':
			recur = 1;
			break;
		case 'k':
			keep = atoi(optarg);
			break;
		case 't':
			tran = atoi(optarg);
			break;
		default:
			ret = -1;
			goto para_err;
			break;
		}
	}

	ret = wac(in_dir, out_file, recur, keep, tran);

done:
indir_err:
outfile_err:
para_err:
	if (out_file != NULL) {
		free(out_file);
		out_file = NULL;
	}

	if (in_dir != NULL) {
		free(in_dir);
		in_dir = NULL;
	}

	return ret;
}