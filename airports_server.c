/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "kdtree.h"

#define pi 3.14159265358979323846

const int treeDimension = 2;

typedef struct node {
	char * airportCode;
	char * city;
	char * state;
	struct node * next;
} node_t;

void initialize(void *kd);

void findNearest(nearest_results_air *result, void *kd, double point[]);

double dist_sq(double *a1, double *a2, int dims);

double distanceFormula(double lat1, double lon1, double lat2, double lon2);

double deg2rad(double deg);

double rad2deg(double rad);

nearest_results_air *
getnearest_air_1_svc(geoLocation *argp, struct svc_req *rqstp)
{
	static nearest_results_air result;
	bool notInitialize = true;

	//xdr_free((xdrproc_t)xdr_nearest_results_place, (char*)&result);

	void *kd, *set, *presults;
	//struct kdres *presults;
	
	double point[2];
	point[0] = argp->longitude;
	point[0] = argp->latitude;

	kd = kd_create(treeDimension);

	if(notInitialize)
	{
		initialize(kd);
	}

	findNearest(&result, kd, point);

	printf("Find airports from these long and lat: \n");
	printf("long: %f\n", argp->longitude);
	printf("lat: %f\n", argp->latitude);

	airList_air walker = result.nearest_results_air_u.x;
	while(walker)
	{
		printf("Code: %s\n", walker->p.code);
		printf("Name: %s\n", walker->p.name);
		printf("Distance: %f\n", walker->p.dist);
		printf("Longitude: %f\n", walker->p.loc.longitude);
		printf("Latitude: %f\n", walker->p.loc.latitude);
		walker = walker->next;
	}

	return &result;
}

void findNearest(nearest_results_air *result, void *kd, double point[])
{
	void *pch;
	double pos[2], distance;
	//presults = kd_nearest2f(kd, 40.352206, -74.657071);
	void *presults = kd_nearest_n(kd, point, 5);
	printf("\n");
	printf("found %d results: \n", kd_res_size(presults));

	if(!presults)
		{ result->err = 0; }
	else
		{ result->err = 1; }
	
	result->nearest_results_air_u.x = NULL;

	while(!kd_res_end(presults)) 
	{
		pch = kd_res_item(presults, pos);
		
		// distance = sqrt( dist_sq(point2, pos, 2));
		distance = distanceFormula(point[0], point[1], pos[0], pos[1]);
		if(!result->nearest_results_air_u.x)
		{
			result->nearest_results_air_u.x = malloc(sizeof(airNode_air));

			result->nearest_results_air_u.x->next = NULL;

			result->nearest_results_air_u.x->p.code = strdup((char*)((struct node*)pch)->airportCode);
			result->nearest_results_air_u.x->p.name = strdup((char*)((struct node*)pch)->city);

			result->nearest_results_air_u.x->p.dist = distance;

			result->nearest_results_air_u.x->p.loc.latitude = pos[0];
			result->nearest_results_air_u.x->p.loc.longitude = pos[1];
			printf("%f\n", distance);
		}
		else
		{
			airList_air temp = malloc(sizeof(airNode_air));

			temp->p.code = strdup((char*)((struct node*)pch)->airportCode);
			temp->p.name = strdup((char*)((struct node*)pch)->city);

			temp->p.dist = distance;
			temp->p.loc.latitude = pos[0];
			temp->p.loc.longitude = pos[1];

			printf("\n");
			printf("Inserting: %f\n", distance);
			printf("Find pos\n");
			airList_air walker = result->nearest_results_air_u.x;
			while(walker->next != NULL && distance > walker->p.dist)
			{
				printf("Current: %f\n", walker->p.dist);
				walker = walker->next;
			}

			printf("Location: %f\n", walker->p.dist);
			if(result->nearest_results_air_u.x->p.dist > distance)
			{
				printf("Add in front: \n");
				temp->next = result->nearest_results_air_u.x;
				result->nearest_results_air_u.x = temp;
			}
			else
			{
				printf("Add in between or end\n");
				temp->next = walker->next;
				walker->next = temp;
			}
		}

		//printf("node at (%.3f, %.3f) is %.3f away and has data = AirportCode:%s City:%s State:%s \n", pos[0], pos[1], distance, (char*)((struct node*)pch)->airportCode, (char*)((struct node*)pch)->city, (char*)((struct node*)pch)->state);

		kd_res_next(presults);
	}

	kd_res_free(presults);
}

void initialize(void *kd)
{
	FILE *fp;
	char myLineBuffer[70];
	char * word;
	ssize_t length;
	bool check;
	char comma = ',';

	// fp = fopen("test.txt", "r");
	fp = fopen("airport-locations.txt", "r");

	if(fp == NULL)
	{
		perror("Error opening file");
	}
	while(fgets(myLineBuffer, 70, fp) != NULL)
	{
		// convert the ssize_t length of character bytes into an integer
		int myLineBufferSize = (int) strlen(myLineBuffer);
		/*
			Create a loop to check to see if there's a comma in the line using a boolean
		*/
		int i;
		for(i = 0; i < myLineBufferSize; i++)
		{
			if(myLineBuffer[i] == comma)
			{
				check = true;
				break;
			}
			check = false;
		}
		/*
			Grab the entire line on the buffer then parse it for each word
			Allocate(on a stack) an array of char that's is dependant on the size of the parsing word
			Need to add a sentinel value('\0') at the end of the allocate char array for termination of the char array size
			Use strncpy(string copy) function call to copy the parse word to the allocate char array
			Store the char array to the pointer 
		*/
		if(check)
		{
			int wordBufferSize;

			// airport code
			word = strtok(myLineBuffer, "[  ] \t ");
			wordBufferSize = (int) strlen(word);
			char airportCode[wordBufferSize];
			airportCode[wordBufferSize] = '\0';
			char * airportCodePointer = airportCode;
			strncpy(airportCode, word, wordBufferSize);
			// printf("Airport Code: %s\n", airportCodePointer);

			// latitude
			word = strtok(NULL, " \t");
			wordBufferSize = (int) strlen(word);
			char latitude[wordBufferSize];
			latitude[wordBufferSize] = '\0';
			strncpy(latitude, word, wordBufferSize);
			float latitudeFloat = atof(latitude);
			// printf("Latitude: %f\n", latitudeFloat);

			// longitude
			word = strtok(NULL, "\t");
			wordBufferSize = (int) strlen(word);
			char longitude[wordBufferSize];
			longitude[wordBufferSize] = '\0';
			strncpy(longitude, word, wordBufferSize);
			float longitudeFloat = atof(longitude);
			// printf("Longitude: %f\n", longitudeFloat);

			// city
			word = strtok(NULL, ",");
			wordBufferSize = (int) strlen(word);
			char city[wordBufferSize];
			city[wordBufferSize] = '\0';
			char * cityPointer = city;
			strncpy(city, word, wordBufferSize);
			// printf("City: %s\n", cityPointer);

			// state
			word = strtok(NULL, "");
			wordBufferSize = (int) strlen(word);
			char state[wordBufferSize];
			state[wordBufferSize] = '\0';
			char * statePointer = state;
			strncpy(state, word, wordBufferSize);
			// printf("State: %s\n", statePointer);

			node_t * head = NULL;
			head = malloc(sizeof(node_t));
			head->airportCode = airportCodePointer;
			head->city = cityPointer;
			head->state = statePointer;
			head->next = NULL;
			kd_insert2f(kd, latitudeFloat, longitudeFloat, head);
			free(head);

		}
	}

	// Need to think where to put this or ask about this
	// kd_free(kd);
	fclose(fp);
}

double dist_sq(double *a1, double *a2, int dims)
{
	double dist_sq = 0, diff;
	while(--dims >= 0)
	{
		diff = (a1[dims] - a2[dims]);
		dist_sq += diff*diff;
	}

	return dist_sq;
}

double distanceFormula(double lat1, double lon1, double lat2, double lon2) 
{   
	double theta, dist;   
	theta = lon1 - lon2;   
	dist = sin(deg2rad(lat1))*sin(deg2rad(lat2))+cos(deg2rad(lat1))*cos(deg2rad(lat2))*cos(deg2rad(theta));   
	dist = acos(dist);   
	dist = rad2deg(dist); 
	dist = dist * 60 * 1.1515;   
	// switch(unit)
	// {     
	// 	case 'M':     
	// 		break;     
	// 	case 'K':       
	// 		dist = dist * 1.609344;   
	// 	break;     
	// 	case 'N':       
	// 		dist = dist * 0.8684;      
	// 	break;
 // 	}   
	return (dist); 
}

double deg2rad(double deg) 
{   
	return (deg * pi / 180); 
}

double rad2deg(double rad) 
{   
	return (rad * 180 / pi); 
}
