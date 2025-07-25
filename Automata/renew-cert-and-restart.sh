#!/bin/bash
# Renew cert and restart nginx

sudo certbot renew --quiet
docker restart nginx