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

/* find absolute value of difference in time between two Points (only meant for same day comparison
 * Different days should not be compared when looking at daily trajectories */
int timedelta(char* p1, char* p2) {
	
	/*get hour, minute, second */
	char* t1 = strdup(p1);
	char* t2 = strdup(p2);
	char *tok;
	char *saveptr;
	int index = 0;
	int p1_hour;
	int p1_minute;
	int p1_second;
	int p2_hour;
	int p2_minute;
	int p2_second;
	/* read t1 */
	for (tok = strtok(t1, ":"); tok && *tok; tok = strtok(NULL, ":")) {
		switch(index) {
			case 0: 
				p1_hour = atoi(tok);
				break;
			case 1:
			        p1_minute = atoi(tok);	
				break;
			case 2 :
			        p1_second = atoi(tok);	
				break ;
		}
		index++;
	}
	index = 0;
	/* read t2 */
        for (tok = strtok_r(t2, ":", &saveptr); tok && *tok; tok = strtok_r(NULL, ":", &saveptr)) {
		switch(index) {
			case 0: 
				p2_hour = atoi(tok);
				break;
			case 1:
			        p2_minute = atoi(tok);	
				break;
			case 2 :
			        p2_second = atoi(tok);	
				break ;
		}
		index++;
	}
	/*convert to integers */
	int p1_time = 3600*p1_hour + 60*p1_minute + p1_second;
	int p2_time = 3600*p2_hour + 60*p2_minute + p2_second;

	int delta = p1_time - p2_time;
	return delta;
}

/* take timedelta , return string in format int [] [hourdelta ,minutedelta ,seconddelta] */
int* parseTimeDelta(int timedelta) {
	static int delta[3]; 
	delta[0] = timedelta / 3600;
	delta[1] = (timedelta%3600)/60;
	delta[2] = (timedelta%3600)%60;
	
	return delta;
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
	p1->time = malloc(strlen("23:34:40") + 1 );
	p2->time = malloc(strlen("23:34:40") + 1 );
	p1->time = "23:34:40";
	p2->time = "21:40:20";	
	printf("%s, %s", p1->time, p2->time);
	printf("\nDistance between points : %f km\n", haversineDistance(p1,p2));

	printf("%s --- %s" , p1->time, p2->time);
	int td = timedelta(p2->time, p1->time);
	int* parseddelta = parseTimeDelta(td);
	printf("\nTime delta : %d seconds", td);
	printf("\nTime delta : %d hours, %d minutes, %d seconds", parseddelta[0], parseddelta[1], parseddelta[2]);
	/* now that you have read in the data .... read cb smot and find out wazzup */


	return 0;
}
