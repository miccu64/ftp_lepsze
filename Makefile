

srv: 
	gcc -o srv serv_02.c child_02.c send_file.c

cli:
	gcc -o cli cliFTP.c send_file.c

rm:
	rm cli srv

		
