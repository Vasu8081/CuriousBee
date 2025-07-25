import os
import json
import argparse
import httpx
import isodate
from sqlalchemy.orm import Session
from app.db.session import get_db
from app.autogen.models.youtube import Youtube

# --------- Constants ---------
YOUTUBE_API_KEY = os.getenv("YOUTUBE_API_KEY")
CHANNEL_ID = os.getenv("YOUTUBE_CHANNEL_ID")


def get_duration_seconds(iso_duration: str) -> int:
    try:
        return int(isodate.parse_duration(iso_duration).total_seconds())
    except Exception:
        return 0


def is_probably_short(duration_seconds: int) -> bool:
    return duration_seconds <= 140


def fetch_youtube_data(limit: int) -> dict:
    if not YOUTUBE_API_KEY or not CHANNEL_ID:
        raise EnvironmentError("Missing YOUTUBE_API_KEY or YOUTUBE_CHANNEL_ID")

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

        search_resp.raise_for_status()
        search_data = search_resp.json()
        video_ids = [item["id"]["videoId"] for item in search_data.get("items", [])]

        detail_resp = client.get(detail_url, params={
            "key": YOUTUBE_API_KEY,
            "id": ",".join(video_ids),
            "part": "snippet,contentDetails"
        })

        detail_resp.raise_for_status()
        return detail_resp.json()


def save_to_json(data: dict, path: str):
    with open(path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2)
    print(f"✅ YouTube data saved to {path}")


def populate_db_from_json(data: dict, db: Session):
    items = data.get("items", [])
    print(f"Processing {len(items)} videos...")

    for item in items:
        snippet = item.get("snippet", {})
        content_details = item.get("contentDetails", {})
        video_id = item.get("id")

        if not snippet or not video_id:
            continue

        duration = get_duration_seconds(content_details.get("duration", ""))
        is_video = not is_probably_short(duration)

        if is_video:
            video = Youtube(
                video_id=video_id,
                title=snippet.get("title", ""),
                description=snippet.get("description", ""),
                published_at=snippet.get("publishedAt", ""),
                thumbnails=snippet.get("thumbnails", {}),
                localized=snippet.get("localized"),
                channel_id=snippet.get("channelId", ""),
                channel_title=snippet.get("channelTitle", ""),
                raw_snippet=snippet,
                is_video=True
            )
            db.merge(video)
            db.commit()
            print(f"🔄 Upserted: {video_id}")
        else:
            print(f"⏭️ Skipped short: {video_id}")


def main():
    parser = argparse.ArgumentParser(description="YouTube data tool")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Download
    download_parser = subparsers.add_parser("download", help="Download data from YouTube API and save as JSON")
    download_parser.add_argument("--output", required=True, help="Path to save the JSON file")
    download_parser.add_argument("--limit", type=int, default=10, help="Number of videos to fetch")

    # Upload
    upload_parser = subparsers.add_parser("upload", help="Upload data from JSON to database")
    upload_parser.add_argument("--input", required=True, help="Path to JSON file to read")

    args = parser.parse_args()

    if args.command == "download":
        data = fetch_youtube_data(args.limit)
        save_to_json(data, args.output)

    elif args.command == "upload":
        with open(args.input, 'r', encoding='utf-8') as f:
            data = json.load(f)
        db = next(get_db())
        try:
            populate_db_from_json(data, db)
        finally:
            db.close()


if __name__ == "__main__":
    main()