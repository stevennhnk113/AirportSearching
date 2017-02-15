/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "places.h"


void
placesprogram_1(char *host, place loc)
{
	CLIENT *clnt;
	nearest_results_place  *result_1;
	place  getnearest_place_1_arg = loc;
	getnearest_place_1_arg = loc;

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

	int i;
	if ( result_1->err == 0 ){
		printf("This is 5 closet airports:\n");
		airList_place walker = result_1->nearest_results_place_u.x;
		for(i = 0; i < 5; i++){
			printf("Airport Code: (%s)  (%s)\n", walker->p.code, walker->p.name);
			printf("Distant: (%d)\n", walker->p.dist);
			printf("(Longitude, Latitude): ( %d, %d)\n", walker->p.longitude, walker->p.latitude);
		}
		clnt_freeres (clnt, (xdrproc_t)xdr_nearest_results_place, (char*)result_1);
	} else {
		printf("There is an error on the server side!!!\n");
	}


#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
  //char *city;
  //char *state;
	place loc;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	loc.city = argv[2];
	loc.state = argv[3];

	printf("city: %s\n", argv[2]);
	printf("state: %s\n", argv[3]);

	placesprogram_1 (host, loc);
	exit (0);
}
