all: libsunacl.so.0

libsunacl.so.0: libsunacl.c acl.h opensolaris_acl.c opensolaris_acl.h
	$(CC) $(CFLAGS) -Wall -pedantic -o libsunacl.so.0 -shared libsunacl.c opensolaris_acl.c -D_ACL_PRIVATE -fPIC

clean:
	rm -f libsunacl.so.0

