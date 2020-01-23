tree: tree.c
	gcc -g -Wall tree.c -o tree -lapr-1 -laprutil-1 -lgcc_s
clean:
	rm tree tree.c~
