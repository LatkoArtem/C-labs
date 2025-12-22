build: lab4.c lab5.c
	gcc -shared -fPIC -o liblab4.so lab4.c
	gcc -shared -fPIC -o liblab5.so lab5.c

run: build
	python3 main.py

clean:
	rm -f liblab4.so liblab5.so