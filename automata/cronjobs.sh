#!/bin/bash

# Paths
AUTOMATA_DIR="$(cd "$(dirname "$0")" && pwd)"
FIREWALL_SCRIPT="$AUTOMATA_DIR/sync-cloudflare-firewall.sh"
CERTBOT_SCRIPT="$AUTOMATA_DIR/renew-cert-and-restart.sh"
CONFIG_FILE="$AUTOMATA_DIR/config.json"

# Identify current hostname (alias in config)
HOSTNAME=$(hostname)

# Determine role from config (requires jq)
ROLE=$(jq -r --arg hn "$HOSTNAME" '.hosts[] | select(.aliases[]? == $hn) | .type' "$CONFIG_FILE")

# Setup cron job strings
FIREWALL_CRON="0 */2 * * * $FIREWALL_SCRIPT >> /var/log/firewall_sync.log 2>&1"
CERTBOT_CRON="0 3 * * 1 $CERTBOT_SCRIPT >> /var/log/certbot_renewal.log 2>&1"

# Read existing crontab
(crontab -l 2>/dev/null | grep -v "$FIREWALL_SCRIPT" | grep -v "$CERTBOT_SCRIPT"; \
  echo "$FIREWALL_CRON"; \
  if [ "$ROLE" = "server" ]; then echo "$CERTBOT_CRON"; fi) | crontab -