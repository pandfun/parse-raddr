main: main.c
	gcc -g main.c

run: main run.sh
	./run.sh
	./sort.sh

clean: 
	rm a.out data final_data
