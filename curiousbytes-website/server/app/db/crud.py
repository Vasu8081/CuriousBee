from sqlalchemy.orm import Session
from sqlalchemy.exc import IntegrityError
from sqlalchemy.dialects.postgresql import insert
from sqlalchemy import func
from app.models.youtube import YouTubeVideo, VideoType
from app.schemas.youtube import YouTubeVideoCreate

# ---------- Upsert ----------
def upsert_video(db: Session, video: YouTubeVideoCreate):
    stmt = insert(YouTubeVideo).values(
        video_id=video.video_id,
        title=video.title,
        description=video.description,
        published_at=video.published_at,
        thumbnails=video.thumbnails,
        localized=video.localized,
        channel_id=video.channel_id,
        channel_title=video.channel_title,
        resources=[dict(r) for r in video.resources],
        raw_snippet=video.raw_snippet,
        is_video = video.is_video
    ).on_conflict_do_update(
        index_elements=['video_id'],
        set_={
            "title": video.title,
            "description": video.description,
            "published_at": video.published_at,
            "thumbnails": video.thumbnails,
            "localized": video.localized,
            "channel_id": video.channel_id,
            "channel_title": video.channel_title,
            "resources": [r.dict() for r in video.resources],
            "raw_snippet": video.raw_snippet,
            "is_video": video.is_video
        }
    )

    try:
        db.execute(stmt)
        db.commit()
    except IntegrityError as e:
        db.rollback()
        raise e

# ---------- Queries ----------
def get_video_by_id(db: Session, video_id: str):
    return db.query(YouTubeVideo).filter(YouTubeVideo.video_id == video_id).first()

def video_exists(db: Session, video_id: str) -> bool:
    return db.query(YouTubeVideo).filter(YouTubeVideo.video_id == video_id).first() is not None

def get_latest_video_timestamp(db: Session):
    return db.query(func.max(YouTubeVideo.published_at)).scalar()

def get_latest_videos(db: Session, limit: int = 5):
    return (
        db.query(YouTubeVideo)
        .order_by(YouTubeVideo.published_at.desc())
        .limit(limit)
        .all()
    )

def get_all_videos(db: Session):
    res = db.query(YouTubeVideo).filter(YouTubeVideo.is_video == True).order_by(YouTubeVideo.published_at.desc()).all()
    print(res)
    return res

from sqlalchemy import and_
def get_videos_by_type(video_type: VideoType, db: Session):
    return (
        db.query(YouTubeVideo)
        .filter(
            and_(
                YouTubeVideo.is_video == True,
                YouTubeVideo.video_type == video_type  # <== uses Enum directly
            )
        )
        .order_by(YouTubeVideo.published_at.desc())
        .all()
    )

from sqlalchemy import or_

def get_all_shorts(db: Session):
    return db.query(YouTubeVideo).filter(
        or_(YouTubeVideo.is_video == False, YouTubeVideo.is_video == None)
    ).order_by(YouTubeVideo.published_at.desc()).all()

def get_all_resources(db: Session):
    videos = get_all_videos(db)
    resources = []
    for video in videos:
        for res in video.resources or []:
            resources.append({
                "video_id": video.video_id,
                "title": video.title,
                "label": res.get("label"),
                "url": res.get("url")
            })
    return resources