all:teludp

teludp:teludp.c
	gcc teludp.c -o teludp
clean:
	rm teludp
