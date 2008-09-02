#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

struct globalArgs_t {
	int noIndex;				/* -I option */
	char *langCode;				/* -l option */
	const char *outFileName;	/* -o option */
	FILE *outFile;
	int verbosity;				/* -v option */
	char **inputFiles;			/* input files */
	int numInputFiles;			/* # of input files */
	int randomized;				/* --randomize option */
} globalArgs;

static const char *optString = "Il:o:vh?";

static const struct option longOpts[] = {
	{ "no-index", no_argument, NULL, 'I' },
	{ "language", required_argument, NULL, 'l' },
	{ "output", required_argument, NULL, 'o' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "randomize", no_argument, NULL, 0 },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

void display_usage( void )
{
	puts( "doc2html - convert documents to HTML" );
	/* ... */
	exit( EXIT_FAILURE );
}

void convert_document( void )
{
	/* ... */
	printf( "no-index: %d\n", globalArgs.noIndex );
	printf( "language: %s\n", globalArgs.langCode );
	printf( "output: %s\n", globalArgs.outFileName );
	printf( "verbosity: %d\n", globalArgs.verbosity );
	printf( "randomized: %d\n", globalArgs.randomized );
}

int getopts( int argc, char *argv[] )
{
	int opt = 0;
	int longIndex = 0;

	/* Initialize globalArgs before we get to work. */
	globalArgs.noIndex = 0;		/* false */
	globalArgs.langCode = NULL;
	globalArgs.outFileName = NULL;
	globalArgs.outFile = NULL;
	globalArgs.verbosity = 0;
	globalArgs.inputFiles = NULL;
	globalArgs.numInputFiles = 0;
	globalArgs.randomized = 0;

	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch( opt ) {
			case 'I':
				globalArgs.noIndex = 1;	/* true */
				break;

			case 'l':
				globalArgs.langCode = optarg;
				break;

			case 'o':
				globalArgs.outFileName = optarg;
				break;

			case 'v':
				globalArgs.verbosity++;
				break;

			case 'h':	/* fall-through is intentional */
			case '?':
				display_usage();
				break;

			case 0:		/* long option without a short arg */
				if( strcmp( "randomize", longOpts[longIndex].name ) == 0 ) {
					globalArgs.randomized = 1;
				}
				break;

			default:
				/* You won't actually get here. */
				break;
		}

		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}

	globalArgs.inputFiles = argv + optind;
	globalArgs.numInputFiles = argc - optind;

	convert_document();

	return EXIT_SUCCESS;
}
