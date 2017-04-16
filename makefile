blktosql: functions.c sha256.c blktosql.c
	gcc -o blktosql functions.c sha256.c blktosql.c -lssl -lcrypto
