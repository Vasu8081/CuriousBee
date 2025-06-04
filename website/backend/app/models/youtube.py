from sqlalchemy import Column, String, Text, DateTime, JSON, Boolean
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class YouTubeVideo(Base):
    __tablename__ = "youtube_videos"

    video_id = Column(String, primary_key=True, index=True)
    title = Column(String)
    description = Column(Text)
    published_at = Column(DateTime)
    thumbnails = Column(JSON)
    localized = Column(JSON)
    channel_id = Column(String)
    channel_title = Column(String)
    resources = Column(JSON)  # List of extracted {label, url} dicts
    raw_snippet = Column(JSON)  # Store full snippet for flexibility
    is_video = Column(Boolean, default=True)  # True = video, False = short