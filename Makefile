CC = gcc
CFLAGS = -Wall -g

OSS = oss
USER = user

all: $(OSS) $(USER)

$(OSS): oss.c
	$(CC) $(CFLAGS) oss.c -o $(OSS)

$(USER): user.c
	$(CC) $(CFLAGS) worker.c -o $(USER)

clean:
	rm -f $(OSS) $(USER) *.o

