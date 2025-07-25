#!/bin/bash

# Load environment variables
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/.env"

# Function to get current public IPv6
get_current_ipv6() {
    curl -6 -s --max-time 10 https://ifconfig.co
}

# Function to get DNS record ID
get_record_id() {
    curl -s -X GET "https://api.cloudflare.com/client/v4/zones/$ZONE_ID/dns_records?type=AAAA&name=$RECORD_NAME" \
        -H "Authorization: Bearer $CF_API_TOKEN" \
        -H "Content-Type: application/json" | jq -r '.result[0].id'
}

# Function to get current DNS IPv6
get_dns_ipv6() {
    dig +short AAAA "$RECORD_NAME"
}

# Main logic
current_ipv6=$(get_current_ipv6)
if [[ -z "$current_ipv6" ]]; then
    echo "Error: Unable to retrieve current IPv6 address."
    exit 1
fi

dns_ipv6=$(get_dns_ipv6)
if [[ "$current_ipv6" == "$dns_ipv6" ]]; then
    echo "No change in IPv6. Current IP is still $current_ipv6"
    exit 0
fi

record_id=$(get_record_id)
if [[ -z "$record_id" || "$record_id" == "null" ]]; then
    echo "Error: Failed to fetch record ID for $RECORD_NAME"
    exit 1
fi

update_response=$(curl -s -X PUT "https://api.cloudflare.com/client/v4/zones/$ZONE_ID/dns_records/$record_id" \
    -H "Authorization: Bearer $CF_API_TOKEN" \
    -H "Content-Type: application/json" \
    --data "{\"type\":\"AAAA\",\"name\":\"$RECORD_NAME\",\"content\":\"$current_ipv6\",\"ttl\":1,\"proxied\":false}")

if echo "$update_response" | grep -q '"success":true'; then
    echo "IPv6 updated to $current_ipv6"
else
    echo "Error: Failed to update DNS record"
    echo "$update_response"
    exit 1
fi

