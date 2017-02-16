/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "places.h"

const int BAD_STATE = -1;
const int UNCLEAR = -2;
const int UNKNOWN = -3;

void
placesprogram_1(char *host, place info)
{
	CLIENT *clnt;
	nearest_results_place  *result_1;
	place getnearest_place_1_arg;
	getnearest_place_1_arg.city = info.city;
	getnearest_place_1_arg.state = info.state;

#ifndef	DEBUG
	clnt = clnt_create (host, placesProgram, places_version, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = getnearest_place_1(&getnearest_place_1_arg, clnt);
	if (result_1 == (nearest_results_place *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else
	{
		int i;
		if ( result_1->err == 0 ){
			printf("This is 5 closet airports:\n");
			airList_place walker = result_1->nearest_results_place_u.x;
			for(i = 0; i < 5; i++){
				printf("%s%s\n", getnearest_place_1_arg.city, getnearest_place_1_arg.state);
				printf("\nAirport Code: (%s)  (%s)\n", walker->p.code, walker->p.name);
				printf("Distant: (%f)\n", walker->p.dist);
				printf("(Longitude, Latitude): ( %f, %f)\n", walker->p.loc.longitude, walker->p.loc.latitude);
				walker = walker->next;
			}
			clnt_freeres (clnt, (xdrproc_t)xdr_nearest_results_place, (char*)result_1);
		} 
		else if (result_1->err == BAD_STATE)
		{
			printf("You input an incorrect State.\n");
		}
		else if (result_1->err == UNCLEAR)
		{
			printf("You need to input a clearer city name.\n");
		}
		else if (result_1->err == UNKNOWN)
		{
			printf("You input an incorrect city\n");
		}
		else 
		{
			printf("There is an error on the server side!!!\n");
		}
	}

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
	place loc;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	loc.city = argv[2];
	loc.state = argv[3];

	placesprogram_1 (host, loc);
	exit (0);
}
