all:
	gcc binaryfile.c -o binaryfile
	gcc scanner.c -o scanner -lcrypto -lssl
	gcc virus.c -o virus

runall:
	./createsBinaryFilesList
	./scanner
	./virus
