docker run -it --rm \
  -v "$(pwd)/certs:/etc/letsencrypt" \
  -v "$(pwd)/certbot-log:/var/log/letsencrypt" \
  certbot/certbot certonly \
  --standalone \
  --preferred-challenges http \
  --agree-tos \
  --no-eff-email \
  -m curiousbytes67@gmail.com \
  -d curiousbytes.in -d www.curiousbytes.in