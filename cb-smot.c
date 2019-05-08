#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

/* This is the C implementation of the Clustering-based approach for Discovering Interesting
 * Places in Trajectories by Palma et al,
 * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.147.3536&rep=rep1&type=pdf
 * The algorithm works by 

*/

/* define struct data point as described in data description docs. 
 * latitude - index 0. .longitude - index 1. altitude - index 3, date - index 5, time - index 6
 * */
struct Point{
	double latitude;
	double longitude;
	double altitude;	
	char* date;
	char* time;
} ;

/* find distance between two Points */ 
double haversineDistance(struct Point* p1, struct Point* p2) {
	/*distance between lat and lon */
	double dlat = (p2->latitude - p1->latitude) * PI / 180.0;
	double dlon = (p2->longitude - p1->longitude) * PI / 180.0;
	/*to rads */
	double lat1 = p1->latitude * PI / 180.0;
	double lat2 = p2->latitude * PI / 180.0;

	double a = pow(sin(dlat / 2), 2) + pow( sin(dlon / 2), 2) * cos(lat1) * cos(lat2);
	double rad = 6371.0;
	double c = 2 * asin(sqrt(a));
	return rad * c;
}

int main() {
	FILE* datafile;
	datafile = fopen("/home/sir/projects/cb-smot/test.plt", "r");
	if (datafile == NULL) {
		printf("Failed opening datafile\n");
		return 1;
	}
	char line[200];
	struct Point* point;
	printf("TESTING");
	while (fgets(line, sizeof line, datafile)) {
		point = (struct Point*)malloc(sizeof(struct Point));
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
	struct Point* p1 = (struct Point*)malloc(sizeof(struct Point)); 
	struct Point* p2 = (struct Point*)malloc(sizeof(struct Point));
	p1->latitude = 51.5007;
	p1->longitude = 0.1246;
	p2->latitude = 40.6892;
	p2->longitude = 74.0445;
	
	printf("\nDistance between points : %f km\n", haversineDistance(p1,p2));

	/* now that you have read in the data .... read cb smot and find out wazzup */


	return 0;
}
