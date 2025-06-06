from datetime import datetime
from typing import List, Optional, Dict
from pydantic import BaseModel, Field

class ResourceLink(BaseModel):
    label: str
    url: str

from enum import Enum

class VideoTypePydantic(str, Enum):
    GateGeneral = "GateGeneral"
    GateStrategy = "GateStrategy"
    GateToppersInterview = "GateToppersInterview"

class YouTubeVideoBase(BaseModel):
    video_id: str
    title: str
    description: str
    published_at: datetime
    channel_id: str
    channel_title: str
    thumbnails: Dict
    localized: Optional[Dict] = None
    resources: List[ResourceLink] = []
    raw_snippet: Optional[Dict] = None
    is_video: bool = True
    video_type: VideoTypePydantic

    model_config = {
        "from_attributes": True
    }

class YouTubeVideoCreate(YouTubeVideoBase):
    pass