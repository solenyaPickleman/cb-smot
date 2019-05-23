#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265
#define VINCENTYMAXLOOPS 1000

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

/* find distance between two Points  using haversine distance*/ 
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

/* find distance between two Points using vincenty solutions of geodesics on the ellipsoid */
double vincentyDistance(struct Point* p1, struct Point* p2) {
	double radius_of_equator = 6378137.0;  /*radius of the equator in meters via WGS-84 */
	double f= 1.0/298.257223563; /* flattening of the elipsoid - WGS-84 */
	double radius_of_poles = (1 - f) * radius_of_equator; 
	double tol = pow(10, -12);
	double latitude1 = p1->latitude * PI / 180.0;
	double latitude2 = p2->latitude * PI / 180.0;
 	double reduced_latitude1 = atan((1 - f) * tan(latitude1));
	double reduced_latitude2 = atan((1 - f) * tan(latitude2));
	double difference_longitude = (p2->longitude*PI / 180.0)  - (p1->longitude * PI / 180.0) ;
	/* set initial lambda value */
	double lambda = difference_longitude; 
	/* precompute to save computation */
	double sin_reduced_latitude1 = sin(reduced_latitude1) ; 
	double cos_reduced_latitude1 = cos(reduced_latitude1) ;
	double sin_reduced_latitude2 = sin(reduced_latitude2) ;
	double cos_reduced_latitude2 = cos(reduced_latitude2) ;	
	/*iterate maxloops times */
	int iterations = 0;
	double cos_lambda;
	double sin_lambda;
	double sin_sigma;
	double cos_sigma;
	double sigma;
	double sin_alpha;
	double cos_sq_alpha;
	double cos2_sigma_m;
	double c;
	double lambda_prev;
	double diff;	
	for ( int i = 0; i < VINCENTYMAXLOOPS; i++) {
		iterations++;
		
		cos_lambda = cos(lambda);
		sin_lambda = sin(lambda);
		sin_sigma = sqrt(pow((cos_reduced_latitude2 * sin_lambda),2) + pow((cos_reduced_latitude1*sin_reduced_latitude2 - sin_reduced_latitude1*cos_reduced_latitude2*cos_lambda),2));
		cos_sigma = sin_reduced_latitude1*sin_reduced_latitude2 + cos_reduced_latitude1*cos_reduced_latitude2*cos_lambda ;	
		sigma =  atan2(sin_sigma, cos_sigma);
		sin_alpha = (cos_reduced_latitude1*cos_reduced_latitude2*sin_lambda)/sin_sigma;
		cos_sq_alpha = 1-pow(sin_alpha,2);
		cos2_sigma_m = cos_sigma - ((2*sin_reduced_latitude1*sin_reduced_latitude2)/cos_sq_alpha);
		c = (f/16)*cos_sq_alpha*(4+f*(4-3*cos_sq_alpha));

		lambda_prev = lambda;
		lambda = difference_longitude + (1-c) * f * sin_alpha * (sigma+c*sin_sigma*(cos2_sigma_m+c*cos_sigma*(-1+2*pow(cos2_sigma_m,2))));

		diff = abs(lambda_prev - lambda);
		if (diff <= tol) {
			break;
		}
	}
	double u_sq = cos_sq_alpha * ((pow(radius_of_equator,2)-pow(radius_of_poles,2))/pow(radius_of_poles,2));
	double A = 1+(u_sq/16384.0) * (4096.0+u_sq * (-768+u_sq*(320-175*u_sq)));
	double B = (u_sq/1024) * (256 + u_sq*(-128 + u_sq*(74-47*u_sq)));
	double delta_sig = B * sin_sigma * (cos2_sigma_m+0.25*B*(cos_sigma*(-1+2*pow(cos2_sigma_m,2))-(1.0/6.0)*B*cos2_sigma_m*(-3+4*pow(sin_sigma,2))*(-3+4*pow(cos2_sigma_m,2))));

	double result_meters = radius_of_poles*A*(sigma - delta_sig);
	return result_meters;
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
	p1->latitude = 39.152501;
	p1->longitude = -84.412977;
	p2->latitude = 39.152505;
	p2->longitude = -84.412946;
	p1->time = malloc(strlen("23:34:40") + 1 );
	p2->time = malloc(strlen("23:34:40") + 1 );
	p1->time = "23:34:40";
	p2->time = "21:40:20";	
	printf("%s, %s", p1->time, p2->time);
	printf("\nDistance between points(haversine) : %f km\n", haversineDistance(p1,p2));
	printf("\nDistance between point(vincenty) : %f m \n" , vincentyDistance(p1, p2));
	printf("%s --- %s\n" , p1->time, p2->time);
	int td = timedelta(p1->time, p2->time);
	int* parseddelta = parseTimeDelta(td);
	printf("\nTime delta : %d seconds\n", td);
	printf("\nTime delta : %d hours, %d minutes, %d seconds\n", parseddelta[0], parseddelta[1], parseddelta[2]);
	/* now that you have read in the data .... read cb smot and find out wazzup */


	return 0;
}
