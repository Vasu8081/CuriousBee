#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/.env"
CONFIG_FILE="$SCRIPT_DIR/config.json"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*"
}

get_current_ipv6() {
    curl -6 -s --max-time 10 https://ifconfig.co
}

get_current_ipv4() {
    curl -4 -s --max-time 10 https://ifconfig.co
}

get_zone_id() {
    domain=$(jq -r '.domain' "$CONFIG_FILE")
    curl -s -H "Authorization: Bearer $CF_API_TOKEN" -H "Content-Type: application/json" \
        "https://api.cloudflare.com/client/v4/zones?name=$domain" |
        jq -r '.result[0].id'
}

get_record_id() {
    local zone_id=$1
    local full_name=$2
    local type=$3
    curl -s -H "Authorization: Bearer $CF_API_TOKEN" -H "Content-Type: application/json" \
        "https://api.cloudflare.com/client/v4/zones/$zone_id/dns_records?type=$type&name=$full_name" |
        jq -r '.result[0].id'
}

update_or_create_record() {
    local zone_id=$1
    local name=$2
    local ip=$3
    local type=$4
    local full="$name.$(jq -r '.domain' "$CONFIG_FILE")"
    local id
    id=$(get_record_id "$zone_id" "$full" "$type")

    if [[ "$id" == "null" || -z "$id" ]]; then
        log "Creating new $type record for $full"
        curl -s -X POST "https://api.cloudflare.com/client/v4/zones/$zone_id/dns_records" \
            -H "Authorization: Bearer $CF_API_TOKEN" \
            -H "Content-Type: application/json" \
            --data "{\"type\":\"$type\",\"name\":\"$full\",\"content\":\"$ip\",\"ttl\":1,\"proxied\":false}" > /dev/null
    else
        log "Updating $type record for $full"
        curl -s -X PUT "https://api.cloudflare.com/client/v4/zones/$zone_id/dns_records/$id" \
            -H "Authorization: Bearer $CF_API_TOKEN" \
            -H "Content-Type: application/json" \
            --data "{\"type\":\"$type\",\"name\":\"$full\",\"content\":\"$ip\",\"ttl\":1,\"proxied\":false}" > /dev/null
    fi
}

get_ipv6_from_cloudflare() {
    local zone_id=$1
    local full_name=$2
    curl -s -H "Authorization: Bearer $CF_API_TOKEN" -H "Content-Type: application/json" \
        "https://api.cloudflare.com/client/v4/zones/$zone_id/dns_records?type=AAAA&name=$full_name" |
        jq -r '.result[0].content'
}

apply_firewall_rules() {
    local allowed_ips=("$@")

    log "Resetting UFW rules..."
    sudo ufw --force reset
    sudo ufw default deny incoming
    sudo ufw default allow outgoing
    sudo ufw allow 443/tcp

    for ip in "${allowed_ips[@]}"; do
        sudo ufw allow from "$ip" to any port 22 proto tcp
    done

    log "Enabling firewall..."
    sudo ufw --force enable
}

main() {
    current_host=$(hostname)
    zone_id=$(get_zone_id)
    current_ipv6=$(get_current_ipv6)
    current_ipv4=$(get_current_ipv4)

    if [[ -z "$current_ipv6" && -z "$current_ipv4" ]]; then
        log "Could not fetch any IP address"
        exit 1
    fi

    log "IPv4: $current_ipv4 | IPv6: $current_ipv6"
    host_entry=$(jq -c --arg name "$current_host" '.hosts[] | select(.name == $name)' "$CONFIG_FILE")

    if [[ -z "$host_entry" ]]; then
        log "Host $current_host not found in config"
        exit 1
    fi

    type=$(echo "$host_entry" | jq -r '.type')
    aliases=($(echo "$host_entry" | jq -r '.aliases[]'))

    # Always add curiousbytes.in and www.curiousbytes.in
    aliases+=("@" "www")

    for alias in "${aliases[@]}"; do
        [[ -n "$current_ipv4" ]] && update_or_create_record "$zone_id" "$alias" "$current_ipv4" "A"
        [[ -n "$current_ipv6" ]] && update_or_create_record "$zone_id" "$alias" "$current_ipv6" "AAAA"
    done

    if [[ "$type" == "server" ]]; then
        allowed_aliases=($(echo "$host_entry" | jq -r '.allow[]'))
        allowed_ips=()

        for alias in "${allowed_aliases[@]}"; do
            full="$alias.$(jq -r '.domain' "$CONFIG_FILE")"
            ip=$(get_ipv6_from_cloudflare "$zone_id" "$full")
            if [[ -n "$ip" && "$ip" != "null" ]]; then
                allowed_ips+=("$ip")
            fi
        done

        apply_firewall_rules "${allowed_ips[@]}"
    fi
}

main "$@"