CFLAGS=-g2 -O0
APPNAME=testapp
all:
	mkdir lib || true
	gcc $(CFLAGS) -shared -fPIC -o lib/libfibonacci.so test_library.c 
	gcc $(CFLAGS) -shared -fPIC httpdlopen.c -ldl -lcurl -o libdlopen.so 
	gcc $(CFLAGS) test.c -ldl -lcurl -L. -ldlopen -o $(APPNAME)

clean:
	rm $(APPNAME) || true
	rm libdlopen.so || true
	rm lib/libfibonacci.so || true
	rm -rf lib || true
