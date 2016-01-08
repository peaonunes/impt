/* TODO: tirar um monte aqui*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sys/timeb.h>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <math.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

/* Lists all files inside a given directory */
int getdir (string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
    	if (strcmp(dirp->d_name,".") != 0 && strcmp(dirp->d_name,"..") != 0)
        	files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

/* Auxiliary time variable */
typedef unsigned long long timestamp_t;

/* Gets current system timestamp_t */
static timestamp_t get_timestamp () {
  struct timeval now;
  gettimeofday (&now, NULL);

  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}


int main() {
	/* ln -s path-to-data data */
	string dir = string("data/");
    vector<string> files = vector<string>();

    getdir(dir,files);

    for (unsigned int i = 0;i < files.size();i++) {
    	
        cout << "\nCreating text indexes...\n";

    	string cmd = "bin/ipmt index data/" + files[i];    
	    
	    cout << "Running " << cmd << "\n";

        timestamp_t t0 = get_timestamp();
		system (cmd.c_str());
        timestamp_t t1 = get_timestamp();	
		
	    double durationInSecs = (t1 - t0) / 1000000.0L;

		cout << "Exec time: " << durationInSecs << "s\n\n";
	}
}

/* TODO: search patterns+indexes */