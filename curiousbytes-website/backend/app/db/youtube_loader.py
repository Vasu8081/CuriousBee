import os
import json
import argparse
import re
from typing import List, Dict

import httpx
from sqlalchemy.orm import Session

from app.db.session import SessionLocal
from app.db.crud import upsert_video, video_exists
from app.schemas.youtube import YouTubeVideoCreate

# --------- Constants ---------
YOUTUBE_API_KEY = os.getenv("YOUTUBE_API_KEY")
CHANNEL_ID = os.getenv("YOUTUBE_CHANNEL_ID")
DEFAULT_LIMIT = 10
DEFAULT_SAVE_PATH = "youtube_data.json"


# --------- Utility ---------
def extract_named_links(description: str) -> List[Dict[str, str]]:
    """Extract labeled URLs like 'Docs: https://example.com' from text."""
    pattern = re.compile(r'^\s*(?P<label>[^:\n]+?)\s*:\s*(?P<url>https?://\S+)', re.MULTILINE)
    return [{"label": label.strip(), "url": url.strip()} for label, url in pattern.findall(description or "")]

import isodate

def get_duration_seconds(iso_duration: str) -> int:
    """Convert ISO 8601 duration to seconds."""
    try:
        duration = isodate.parse_duration(iso_duration)
        return int(duration.total_seconds())
    except Exception:
        return 0

def is_probably_short(duration_seconds: int) -> bool:
    """Determine if video is likely a short based on duration."""
    return duration_seconds <= 140


# --------- Fetch YouTube Data from API ---------
def fetch_youtube_data(limit: int = DEFAULT_LIMIT) -> dict:
    if not YOUTUBE_API_KEY or not CHANNEL_ID:
        raise EnvironmentError("Missing YOUTUBE_API_KEY or YOUTUBE_CHANNEL_ID")

    print(f"Fetching {limit} videos from YouTube API...")

    base_url = "https://www.googleapis.com/youtube/v3/search"
    detail_url = "https://www.googleapis.com/youtube/v3/videos"

    with httpx.Client(timeout=10) as client:
        search_resp = client.get(base_url, params={
            "key": YOUTUBE_API_KEY,
            "channelId": CHANNEL_ID,
            "part": "snippet",
            "order": "date",
            "maxResults": limit,
            "type": "video"
        })

        if search_resp.status_code != 200:
            raise Exception(f"[YouTube Search API] Error: {search_resp.text}")

        search_data = search_resp.json()
        video_ids = [item["id"]["videoId"] for item in search_data.get("items", []) if "videoId" in item["id"]]

        if not video_ids:
            raise Exception("No video IDs found.")

        detail_resp = client.get(detail_url, params={
            "key": YOUTUBE_API_KEY,
            "id": ",".join(video_ids),
            "part": "snippet, contentDetails"
        })

        if detail_resp.status_code != 200:
            raise Exception(f"[YouTube Detail API] Error: {detail_resp.text}")

        return detail_resp.json()


# --------- Populate DB from YouTube Data ---------
def populate_from_json(data: dict, db: Session):
    items = data.get("items", [])
    print(f"Processing {len(items)} videos...")

    for item in items:
        snippet = item.get("snippet", {})
        content_details = item.get("contentDetails", {})
        video_id = item.get("id")
        
        if not snippet or not video_id:
            continue

        duration_iso = content_details.get("duration", "")
        duration_seconds = get_duration_seconds(duration_iso)

        is_video = not is_probably_short(duration_seconds)
        print(snippet.get("title", ""), duration_seconds, is_video)
        # Build the video schema
        video_data = YouTubeVideoCreate(
            video_id=video_id,
            title=snippet.get("title", ""),
            description=snippet.get("description", ""),
            published_at=snippet.get("publishedAt", ""),
            thumbnails=snippet.get("thumbnails", {}),
            localized=snippet.get("localized"),
            channel_id=snippet.get("channelId", ""),
            channel_title=snippet.get("channelTitle", ""),
            resources=extract_named_links(snippet.get("description", "")),
            raw_snippet=snippet,
            is_video = is_video
        )

        # Always upsert (insert or update)
        upsert_video(db, video_data)
        print(f"🔄 Upserted video: {video_id}")

# --------- Main CLI ---------
def main():
    parser = argparse.ArgumentParser(description="Populate YouTube videos into the database.")
    parser.add_argument('--json', help='Path to JSON file to import from (skips API call)')
    parser.add_argument('--limit', type=int, default=DEFAULT_LIMIT, help='Number of videos to fetch from YouTube API')
    parser.add_argument('--save', action='store_true', help='Save fetched YouTube data to a local file')

    args = parser.parse_args()
    db = SessionLocal()

    try:
        if args.json:
            print(f"Loading data from JSON: {args.json}")
            with open(args.json, 'r', encoding='utf-8') as f:
                data = json.load(f)
        else:
            data = fetch_youtube_data(limit=args.limit)
            if args.save:
                with open(DEFAULT_SAVE_PATH, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
                print(f"📁 Saved API data to {DEFAULT_SAVE_PATH}")

        populate_from_json(data, db)

    finally:
        db.close()


if __name__ == "__main__":
    main()