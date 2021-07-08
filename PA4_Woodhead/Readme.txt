pager-lru.c
	Contains my code for a basic lru pager, removes only the least recently used page inside each process not globally

pager-predict.c
	Contains my code for a predictive pager, works by finding the most come page that comes after each page and the loading that into frame 100 ticks before it needs to be used

to run either of my implmentations use
	$ make
	then
	./test-lru
	./test-predict