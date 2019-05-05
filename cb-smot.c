#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This is the C implementation of the Clustering-based approach for Discovering Interesting
 * Places in Trajectories by Palma et al,
 * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.147.3536&rep=rep1&type=pdf
 * The algorithm works by 

*/

/* define struct data point as described in data description docs. 
 * latitude - index 0. .longitude - index 1. altitude - index 3, date - index 5, time - index 6
 * */
typedef struct {
	double latitude;
	double longitude;
	double altitude;	
	char* date;
	char* time;
} Point;


int main() {
	FILE* datafile;
	datafile = fopen("/home/sir/projects/cb-smot/test.plt", "r");
	if (datafile == NULL) {
		printf("Failed opening datafile\n");
		return 1;
	}
	char line[200];
	Point* point;
	printf("TESTING");
	while (fgets(line, sizeof line, datafile)) {
		point = (Point*)malloc(sizeof(Point));
		int index = 0;
		char *tok;
		char *saveptr; 
		for (tok = strtok_r(line, ",", &saveptr); tok && *tok; tok = strtok_r(NULL, ",", &saveptr)) { 
			switch(index) {
				case 0:
					point->latitude= atof(tok);
					break;
				case 1:
					point->longitude= atof(tok);
					break;
				case 3:
					point->altitude= atof(tok);
					break;
				case 5:
					point->date = malloc(strlen(tok) + 1);
					strcpy(point->date, tok);
					break;
				case 6:
					point->time = malloc(strlen(tok)+1);
					strcpy(point->time, tok);
					break;
			}	
			index++;
		}
			printf("latitude : %.9f, longitude: %.9f %s  %s\n", point->latitude, point->longitude, point->date, point->time);
	}

	fclose(datafile);
	printf("CB-SMOT\n");
	return 0;
}
