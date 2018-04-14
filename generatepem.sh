openssl req -nodes -new -x509 -keyout server.key -out server.cert
cat server.key server.cert > ssl.pem
rm server.key
rm server.cert