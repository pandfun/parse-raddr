#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


#define MAPS_SIZE 900000

#define PAGEMAP_BYTE_SIZE 8
#define PAGE_SIZE getpagesize()

typedef unsigned long U64;


/*
 * Function to translate the virtual addr to physical addr for chunk of vaddr
 * between 'start_vaddr' and 'end_vaddr' and store them in a database (file).
 */
void find_raddr(int pid, U64 start_vaddr, U64 end_vaddr, FILE *db, FILE *pagemap)
{
	int rc, i;

	U64 raddr, vaddr, offset;

	for (vaddr = start_vaddr; vaddr <= end_vaddr; vaddr += PAGE_SIZE) {
		
		offset = (vaddr / PAGE_SIZE) * PAGEMAP_BYTE_SIZE;

		rc = fseek(pagemap, offset, SEEK_SET);
		if (rc < 0)
			perror("fseek"), exit(1);

		fread(&raddr, PAGEMAP_BYTE_SIZE, 1, pagemap);

		raddr &= 0x007FFFFFFFFFFFFF; // To store only the addr and ignore the flags
		if (raddr == 0)
			continue;

		fprintf(db, "%d %llx   %llx\n", pid, vaddr, raddr);
	}

	return;
}


/*
 * Function will find the ranges of vitual addr chunks 
 * by reading the maps file, and then call 'find_raddr' for that chunk.
 */
void read_maps(int pid, char *buff, FILE *db, FILE *pagemap)
{
	if (buff == 0 || *buff == '\0') {
		printf("Maps not found!\n");
		exit(1);
	}
	
	U64 start_vaddr, end_vaddr;

	char *start, *end;
	start = buff;
	

	do {
		end = 0;
		end = strchr(start, '-');
		if (!end)
			printf("Error: Cannot find '-' in buffer!\n"), exit(1);
		*end = '\0';

		start_vaddr = strtoull(start, NULL, 16);

		start = end + 1;

		end = 0;
		end = strchr(start, ' ');
		if (!end)
			printf("Error: Cannot find ' ' in buffer!\n"), exit(1);
		*end = '\0';

		end_vaddr = strtoull(start, NULL, 16);

		find_raddr(pid, start_vaddr, end_vaddr, db, pagemap);

		while (*start != '\n')
			++start;

		++start;

	} while (*start != '\0');
}



int main(int argc, char *argv[])
{
	if (argc != 2)
		printf("Usage: %s <PID>\n", argv[0]), exit(1);

	int rc;

	char maps_path[32] = {0};
	snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", argv[1]);

	FILE *maps = fopen(maps_path, "rb");
	if (!maps) {
		printf("Cannot open: %s\n", maps_path);
		perror("fopen");
		exit(1);
	}

	U64 maps_size = MAPS_SIZE;
	char *maps_buff = malloc(maps_size * sizeof(char));
	memset(maps_buff, 0, sizeof(maps_buff));

	rc = fread(maps_buff, sizeof(char), maps_size, maps);
	if (rc < 0) {
		perror("Failed to load maps");
		exit(1);
	}
	
	char pagemap_path[32] = {0};
	snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%s/pagemap", argv[1]);
	FILE *pagemap = fopen(pagemap_path, "rb");

	FILE *db = fopen("/home/krish/Projects/parse_raddr/data", "a+");

	read_maps(atoi(argv[1]), maps_buff, db, pagemap);

	return 0;
}

