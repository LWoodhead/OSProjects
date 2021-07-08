/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */


//working predict
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
    static int previous_page[MAXPROCESSES];//Store the previous page the process was using
    static int predictor[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];//Store how many times one page follows another for each process
    //printf("%s\n %d", "pageit called",tick);
    /* Local vars */
    int proctmp;
    int pagetmp;
    int page,oldest_timestamp,oldest_page,prediction_page,second_oldest_page,searched;

    /* initialize static vars on first run */
    if(!initialized){
    for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
        previous_page[proctmp] = -1;
        for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
            timestamps[proctmp][pagetmp] = 0;
            for(int i = 0; i < MAXPROCPAGES; i++){
                predictor[proctmp][pagetmp][i] = -1;
            } 
        }
    }
    initialized = 1;
    }
    
    //Loop through all processes
    for(int proc = 0; proc < MAXPROCESSES; proc++){
        //Check to see if the process is active
        searched = -1;
        if(q[proc].active){
            //For each process determine page with pc/pagesize
            page = q[proc].pc/PAGESIZE;
            oldest_timestamp = tick;
            oldest_page = page;
            second_oldest_page = page;
            //check if the page is swapped in 
            if(!q[proc].pages[page]){
                //if it isnt swapped in try to call pagein and swap it in
                if(!pagein(proc,page)){
                    //If the pagein fails we must find and evict a page
                    searched = 1;
                    for(int j = 0;j < MAXPROCPAGES; j++){
                        //Look to find the oldest time stamped page that is currently loaded into memory
                        if(timestamps[proc][j] < oldest_timestamp && q[proc].pages[j]){
                            oldest_timestamp = timestamps[proc][j];
                            second_oldest_page = oldest_page;
                            oldest_page = j;
                        }
                    }
                    pageout(proc,oldest_page);
                    timestamps[proc][page] = tick;
                }
            }else{
                timestamps[proc][page] = tick;
            }
            
            //Assign a predictive page
            prediction_page = 0;
            int count = 0;
            for(int i = 0; i < MAXPROCPAGES; i++){
                if(predictor[proc][(q[proc].pc+100)/PAGESIZE][i] >= count){
                    count = predictor[proc][(q[proc].pc+100)/PAGESIZE][i];
                    prediction_page = i;
                }
            }
            oldest_timestamp = tick;
            oldest_page = prediction_page;
            //If page is not already swapped in
            if(!q[proc].pages[prediction_page]){
                //Try to swap in
                if(!pagein(proc,prediction_page)){
                    //If the pagein fails we must find and evict a page
                    if(searched != 1){
                        for(int j = 0;j < MAXPROCPAGES; j++){
                            //Look to find the oldest time stamped page that is currently loaded into memory
                            if(timestamps[proc][j] < oldest_timestamp && q[proc].pages[j]){
                                oldest_timestamp = timestamps[proc][j];
                                oldest_page = j;
                            }
                        }
                    }else{
                        oldest_page = second_oldest_page;
                    }
                    if(oldest_page != page){
                        pageout(proc,oldest_page);
                    }
                    timestamps[proc][prediction_page] = tick;
                }else{
                    timestamps[proc][prediction_page] = tick;
                }
            }
            //Add to the predictor and previous_page 
            if(previous_page[proc] != -1){
                predictor[proc][previous_page[proc]][page]++;
            }
            previous_page[proc] = page;
        }
    }
    /* advance time for next pageit iteration */
    tick++;
} 