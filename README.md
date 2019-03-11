# Network-Security-Virus-scanner

Compile files:
$ make run

Run files:
$ make runall

note 1:
- Few binary files are already present in the .zip folder.

- ./new1 and ./new3 are injected where as ./new2 and ./new4 are not

- after make runall the scanner will notify about these two infected files

- open another terminal and type './virus'

- now scanner program will print all the injected files.

note 2: 
- ./virus is commented out in makefile under runall section. Once ./virus is run, it will inject BUSTED and DEADBEEFDEAD signature in all the binary files present in the current directory.

- In order to keep the difference I have provided few infected and few non infected binary files initially.
