CC = gcc

SPRSIMUL = ssu_PR_simulator

all : $(SPRSIMUL)

$(SPRSIMUL) : ssu_PR_simulator.c
	$(CC) -o $@ $^ func.c Optimal.c FIFO.c LIFO.c LRU.c LFU.c SC.c ESC.c
	
clean:
	rm -rf *.o
	rm -rf $(SPRSIMUL)

