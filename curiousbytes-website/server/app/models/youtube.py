from enum import Enum
from sqlalchemy import Column, String, Text, DateTime, JSON, Boolean, Enum as SQLEnum
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class VideoType(str, Enum):
    GateGeneral = "GateGeneral"
    GateStrategy = "GateStrategy"
    GateToppersInterview = "GateToppersInterview"

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
    resources = Column(JSON)
    raw_snippet = Column(JSON)
    is_video = Column(Boolean, default=True)
    video_type = Column(SQLEnum(VideoType, name="videotype"), nullable=False, default=VideoType.GateGeneral)