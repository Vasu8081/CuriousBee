import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, TEXT, Float, Date, Time, DateTime, Boolean, LargeBinary, JSON
from app.db.base import Base
from typing import Optional, cast
from sqlalchemy import Enum as SQLEnum
from app.autogen.models.enums import *

class Youtube(Base):
    __tablename__ = 'youtube'

    video_id = Column(String, primary_key=True, default=uuid.uuid4)
    title = Column(String)
    description = Column(TEXT)
    published_at = Column(DateTime)
    thumbnails = Column(JSON)
    localized = Column(JSON)
    channel_id = Column(String)
    channel_title = Column(String)
    raw_snippet = Column(JSON)
    is_video = Column(Boolean)
    video_type = Column(SQLEnum(VideoTypes, name='video_types'), nullable=False, default=VideoTypes.Gate)

    @property
    def VideoId(self) -> str:
        return cast(str, self.video_id)

    @VideoId.setter
    def VideoId(self, value: Optional[str]) -> None:
        self.video_id = value

    @property
    def Title(self) -> str:
        return cast(str, self.title)

    @Title.setter
    def Title(self, value: Optional[str]) -> None:
        self.title = value

    @property
    def Description(self) -> str:
        return cast(str, self.description)

    @Description.setter
    def Description(self, value: Optional[str]) -> None:
        self.description = value

    @property
    def PublishedAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.published_at)

    @PublishedAt.setter
    def PublishedAt(self, value: Optional[datetime.datetime]) -> None:
        self.published_at = value

    @property
    def Thumbnails(self) -> dict:
        return cast(dict, self.thumbnails)

    @Thumbnails.setter
    def Thumbnails(self, value: Optional[dict]) -> None:
        self.thumbnails = value

    @property
    def Localized(self) -> dict:
        return cast(dict, self.localized)

    @Localized.setter
    def Localized(self, value: Optional[dict]) -> None:
        self.localized = value

    @property
    def ChannelId(self) -> str:
        return cast(str, self.channel_id)

    @ChannelId.setter
    def ChannelId(self, value: Optional[str]) -> None:
        self.channel_id = value

    @property
    def ChannelTitle(self) -> str:
        return cast(str, self.channel_title)

    @ChannelTitle.setter
    def ChannelTitle(self, value: Optional[str]) -> None:
        self.channel_title = value

    @property
    def RawSnippet(self) -> dict:
        return cast(dict, self.raw_snippet)

    @RawSnippet.setter
    def RawSnippet(self, value: Optional[dict]) -> None:
        self.raw_snippet = value

    @property
    def IsVideo(self) -> bool:
        return cast(bool, self.is_video)

    @IsVideo.setter
    def IsVideo(self, value: Optional[bool]) -> None:
        self.is_video = value

    @property
    def VideoType(self) -> VideoTypes:
        return cast(VideoTypes, self.video_type)

    @VideoType.setter
    def VideoType(self, value: Optional[VideoTypes]) -> None:
        self.video_type = value

    def to_schema(self):
        from app.autogen.schemas.youtube_schema import YoutubeSchema
        return YoutubeSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
