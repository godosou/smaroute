#include "patMapMatchingBed.h"
#include "patListDirectory.h"
#include "initParams.h"
#include "patNBParameters.h"
int main(int argc, char *argv[]) {

	initParameters (argv[1]);
	vector<string> file_list;
	patListDirectory ld;
	ld.getFilesInDeep(patNBParameters::the()->dataDirectory, ".csv", file_list);

	patMapMatchingBed mmb(file_list);
	mmb.genShortestPath();
}
