blktosql: functions.c sha256.c blktosql.c
	gcc -o blktosql functions.c sha256.c blktosql.c base58.c -lssl -lcrypto
