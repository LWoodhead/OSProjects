/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    //printf("%s\n %d", "pageit called",tick);
    /* Local vars */
    int proctmp;
    int pagetmp;
    int page,oldest_timestamp,oldest_page;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    // fprintf(stderr, "pager-lru not yet implemented. Exiting...\n");
    // exit(EXIT_FAILURE);
    //Loop through all processes
    for(int proc = 0; proc < MAXPROCESSES; proc++){
        //Check to see if the process is active
        if(q[proc].active){
            //For each process determine page with pc/pagesize
            page = q[proc].pc/PAGESIZE;
            oldest_timestamp = tick;
            oldest_page = page;
            //check if the page is swapped in 
            if(!q[proc].pages[page]){
                //if it isnt swapped in try to call pagein and swap it in
                if(!pagein(proc,page)){
                    //If the pagein fails we must find and evict a page
                    for(int j = 0;j < MAXPROCPAGES; j++){
                        //Look to find the oldest time stamped page that is currently loaded into memory
                        if(timestamps[proc][j] < oldest_timestamp && q[proc].pages[j]){
                            oldest_timestamp = timestamps[proc][j];
                            oldest_page = j;
                        }
                    }
                    pageout(proc,oldest_page);
                }
            }
            //Set the timestamp for the current page to be swapped in so it is the most recently used page
            timestamps[proc][page] = tick;
        }
    }
    /* advance time for next pageit iteration */
    tick++;
} 
